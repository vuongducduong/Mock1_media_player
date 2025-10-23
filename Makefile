CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
LDFLAGS = -lncursesw -ltinfo -lSDL2 -lSDL2_mixer -ltag

SRC_DIRS = controllers models views
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/media_player

# Tự động tìm tất cả file .cpp
SRCS = $(wildcard main.cpp $(foreach dir,$(SRC_DIRS),$(dir)/*.cpp))
# Chuyển .cpp thành .o trong thư mục build tương ứng
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

# Màu in ra terminal
YELLOW = \033[1;33m
GREEN = \033[1;32m
RESET = \033[0m

# Rule mặc định
all: $(TARGET)

# Link tất cả object file thành binary
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo -e "$(YELLOW)[Linking]$(RESET) $@"
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo -e "$(GREEN)Build successful!$(RESET)"

# Compile từng file .cpp thành .o
$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)[Compiling]$(RESET) $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dọn dẹp
clean:
	@echo -e "$(YELLOW)[Cleaning build files]$(RESET)"
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo -e "$(GREEN)Clean done.$(RESET)"

.PHONY: all clean
