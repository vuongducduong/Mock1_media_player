#ifndef USBMANAGER_H
#define USBMANAGER_H

#include <string>
#include <vector>
#include <mutex>
#include <atomic>

class UsbManager {
public:
    UsbManager();

    void updateStatus();
    void ejectAll();
    bool hasUsb() const;
    std::string getStatusText() const;
    std::string getUsbName() const;
    bool isUsbConnected() const { return usbConnected.load(); }


private:
    // === NEW ===
    void copyAllMp3ToLocal(const std::string& mountPath);
    void copyAllMp3ToLocalAsync(const std::string& mountPath);
    void clearLocalUsbFolder();
    std::string detectUsbMountPath() const;

    // internal utils
    std::string execCmd(const std::string& cmd) const;
    std::vector<std::string> getPartitions() const;
    std::vector<std::string> getPartitionsOfParent(const std::string& parentDev) const;
    std::string getMountPoint(const std::string& dev) const;
    bool safeMount(const std::string& dev);
    void safeUnmount(const std::string& dev);
    void safeDetach(const std::string& dev);

private:
    mutable std::mutex mtx;
    std::atomic<bool> usbConnected;
    std::atomic<bool> ejectInProgress;
    std::vector<std::string> connectedDevs;
    std::string lastAction;
};

#endif
