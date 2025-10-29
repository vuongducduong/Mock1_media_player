#include "UsbManager.h"
#include <array>
#include <regex>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

UsbManager::UsbManager() 
    : usbConnected(false),
      ejectInProgress(false),
      lastAction("Ready") {}

// Executes a command and returns trimmed stdout (non-blocking safety: use popen)
std::string UsbManager::execCmd(const std::string& cmd) const {
    std::array<char, 256> buffer{};
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    int rc = pclose(pipe); (void)rc;
    // trim right
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ' || result.back() == '\t'))
        result.pop_back();
    return result;
}

// return all partition device names which belong to usb transport
std::vector<std::string> UsbManager::getPartitions() const {
    std::vector<std::string> devs;
    FILE* pipe = popen("lsblk -lnpo NAME,TYPE | awk '$2==\"part\" {print $1}'", "r");
    if (!pipe) return devs;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string dev(buffer);
        while (!dev.empty() && (dev.back()=='\n' || dev.back()=='\r' || dev.back()==' ' || dev.back()=='\t')) dev.pop_back();
        if (dev.empty()) continue;

        // find parent (PKNAME) and check TRAN
        std::string parent = execCmd(std::string("lsblk -no PKNAME ") + dev);
        if (parent.empty()) continue;
        std::string tran = execCmd(std::string("lsblk -no TRAN /dev/") + parent);
        if (tran == "usb") devs.push_back(dev);
    }
    pclose(pipe);
    return devs;
}

// return partition list for a given parent device name (e.g. sde -> /dev/sde1)
std::vector<std::string> UsbManager::getPartitionsOfParent(const std::string& parentDev) const {
    std::vector<std::string> out;
    // lsblk -lnpo NAME,PKNAME | awk '$2=="sde" {print $1}'
    std::string cmd = std::string("lsblk -lnpo NAME,PKNAME | awk '$2==\"") + parentDev + "\" {print $1}'";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return out;
    char buf[256];
    while (fgets(buf, sizeof(buf), pipe) != nullptr) {
        std::string s(buf);
        while (!s.empty() && (s.back()=='\n' || s.back()=='\r' || s.back()==' ' || s.back()=='\t')) s.pop_back();
        if (!s.empty()) out.push_back(s);
    }
    pclose(pipe);
    return out;
}

std::string UsbManager::getMountPoint(const std::string& dev) const {
    return execCmd(std::string("lsblk -no MOUNTPOINT ") + dev);
}

// Try mount with udisksctl; update lastAction inside mutex
bool UsbManager::safeMount(const std::string& dev) {
    // check existing mount
    std::string mp = getMountPoint(dev);
    if (!mp.empty()) {
        std::lock_guard<std::mutex> lock(mtx);
        lastAction = "Already mounted " + dev;
        return true;
    }

    int ret = system((std::string("udisksctl mount -b ") + dev + " >/dev/null 2>&1").c_str());
    system("sync");
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (ret == 0) {
            mp = getMountPoint(dev);
            // optionally remount with sync/noatime if mountpoint exists
            if (!mp.empty()) {
                system((std::string("mount -o remount,sync,noatime ") + dev + " " + mp + " >/dev/null 2>&1").c_str());
            }
            lastAction = "Mounted " + dev;
        } else {
            lastAction = "Mount failed " + dev;
        }
    }
    return (ret == 0);
}

void UsbManager::safeUnmount(const std::string& dev) {
    std::string mp = getMountPoint(dev);
    if (mp.empty()) {
        std::lock_guard<std::mutex> lock(mtx);
        lastAction = "Not mounted: " + dev;
        return;
    }
    int ret = system((std::string("udisksctl unmount -b ") + dev + " >/dev/null 2>&1").c_str());
    system("sync");
    std::lock_guard<std::mutex> lock(mtx);
    lastAction = (ret == 0) ? ("Unmounted " + dev) : ("Unmount failed " + dev);
}

void UsbManager::safeDetach(const std::string& dev) {
    // dev is partition (/dev/sde1) -> find parent name
    std::string parentName = execCmd(std::string("lsblk -no PKNAME ") + dev);
    if (parentName.empty()) {
        // fallback: extract after last '/'
        auto pos = dev.find_last_of('/');
        if (pos == std::string::npos || pos + 1 >= dev.size()) {
            std::lock_guard<std::mutex> lock(mtx);
            lastAction = "Cannot determine parent for " + dev;
            return;
        }
        parentName = dev.substr(pos + 1);
    }

    // Ensure all partitions of parent are unmounted first
    auto parts = getPartitionsOfParent(parentName);
    for (const auto& p : parts) {
        safeUnmount(p);
    }

    // power-off parent device
    std::string parentDevPath = std::string("/dev/") + parentName;
    int ret = system((std::string("udisksctl power-off -b ") + parentDevPath + " >/dev/null 2>&1").c_str());
    system("sync");
    std::lock_guard<std::mutex> lock(mtx);
    lastAction = (ret == 0) ? ("Safely removed " + parentDevPath) : ("Power-off failed " + parentDevPath);
}

void UsbManager::updateStatus() {
    if (ejectInProgress.load()) return;

    auto parts = getPartitions();
    bool hasNow = !parts.empty();
    bool hadBefore = usbConnected.load();

    if (hasNow && !hadBefore) {
        // USB vừa được cắm vào
        {
            std::lock_guard<std::mutex> lock(mtx);
            connectedDevs = parts;   // 🔥 luôn cập nhật từ getPartitions()
            usbConnected = true;
            lastAction = "USB inserted, waiting for mount...";
        }

        // 🔥 chờ 1s để hệ thống nhận diện đầy đủ partition và udisksd sẵn sàng
        std::this_thread::sleep_for(std::chrono::seconds(1));

        for (const auto& d : parts) {
            safeMount(d);
        }

        std::string mountPath = detectUsbMountPath();
        if (!mountPath.empty()) {
            copyAllMp3ToLocal(mountPath);
        }
    } 
    else if (!hasNow && hadBefore) {
        // USB vừa bị rút
        {
            std::lock_guard<std::mutex> lock(mtx);
            connectedDevs.clear();
            usbConnected = false;
            lastAction = "USB removed";
        }

        clearLocalUsbFolder();
    }
}

bool UsbManager::hasUsb() const {
    return usbConnected.load();
}

std::string UsbManager::getStatusText() const {
    std::lock_guard<std::mutex> lock(mtx);
    return lastAction;
}

void UsbManager::ejectAll() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastAction = "Eject started";
        ejectInProgress = true;
    }

    std::vector<std::string> copy;
    {
        std::lock_guard<std::mutex> lock(mtx);
        copy = connectedDevs;
    }

    std::thread([this, copy]() {
        for (const auto& d : copy) {
            safeUnmount(d);
            safeDetach(d);
        }

        clearLocalUsbFolder();

        {
            std::lock_guard<std::mutex> lock(mtx);
            connectedDevs.clear();
            usbConnected = false;
            lastAction = "Safely removed. ./usb cleared.";
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));

        {
            std::lock_guard<std::mutex> lock(mtx);
            lastAction = "Not detected";
            ejectInProgress = false;
        }

    }).detach();
}

std::string UsbManager::getUsbName() const {
    std::lock_guard<std::mutex> lock(mtx);
    if (!connectedDevs.empty()) {
        // show first partition (you can improve to get LABEL if needed)
        return connectedDevs.front();
    }
    return std::string("No Device");
}

// =================================================================
// === NEW: Tìm mount path thật sự của USB ===
std::string UsbManager::detectUsbMountPath() const {
    // lấy mount point của partition đầu tiên
    if (connectedDevs.empty()) return "";
    std::string mp = getMountPoint(connectedDevs.front());
    if (mp.empty()) {
        // fallback: tìm mount usb trong /media hoặc /run/media
        FILE* pipe = popen("lsblk -lnpo MOUNTPOINT,TRAN | awk '$2==\"usb\" {print $1; exit}'", "r");
        if (!pipe) return "";
        char buf[256];
        std::string result;
        if (fgets(buf, sizeof(buf), pipe)) result = buf;
        pclose(pipe);
        while (!result.empty() && (result.back()=='\n' || result.back()==' ')) result.pop_back();
        return result;
    }
    return mp;
}

// =================================================================
// === NEW: Đệ quy copy tất cả file .mp3 từ USB sang ./usb ===
void UsbManager::copyAllMp3ToLocal(const std::string& mountPath) {
    if (mountPath.empty() || !fs::exists(mountPath)) return;

    const fs::path destRoot("./usb");

    // Xóa thư mục cũ trước khi copy
    if (fs::exists(destRoot)) fs::remove_all(destRoot);
    fs::create_directories(destRoot);

    size_t copiedCount = 0;

    for (auto& p : fs::recursive_directory_iterator(mountPath)) {
        if (p.is_regular_file() && p.path().extension() == ".mp3") {
            try {
                fs::path dest = destRoot / p.path().filename(); // ✅ chỉ giữ tên file
                fs::copy_file(p.path(), dest, fs::copy_options::overwrite_existing);
                copiedCount++;
            } catch (const std::exception& e) {
                std::lock_guard<std::mutex> lock(mtx);
                lastAction = std::string("Copy failed: ") + e.what();
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        if (copiedCount == 0)
            lastAction = "No .mp3 files found on USB";
        else
            lastAction = "Copied " + std::to_string(copiedCount) + " .mp3 files to ./usb";
    }
}

// =================================================================
// === NEW: Dọn sạch ./usb khi Eject ===
void UsbManager::clearLocalUsbFolder() {
    try {
        fs::path destRoot("./usb");
        if (fs::exists(destRoot)) fs::remove_all(destRoot);
        fs::create_directories(destRoot);
    } catch (...) {}
}
