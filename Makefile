# Detect Windows environment
ifeq ($(OS),Windows_NT)
    WINDOWS := 1
else
    WINDOWS := 0
endif

PKGS := sdl3 sdl3-image sdl3-ttf

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -O3 -m64 -Isrc -s `pkg-config --cflags $(PKGS)`
LDFLAGS := -Wl,--subsystem,console `pkg-config --libs $(PKGS)` -lwebsockets
# -lssl -lcrypto -lz -lpthread

# Add -lmingw32 first for Windows
ifeq ($(WINDOWS), 1)
# CXXFLAGS += -Id:/src/cpp/include -Ld:/src/cpp/lib -s
    LDFLAGS := -lmingw32 -lws2_32 $(LDFLAGS)
endif

# Build directories
BUILD_DIR := build

GAME_SRC := $(wildcard src/game/*.cpp)
WAR2_SRC := war2.cpp $(GAME_SRC) $(wildcard src/war2/*.cpp)
KLAD1_SRC := klad1.cpp $(GAME_SRC) $(wildcard src/klad1/*.cpp)

WAR2_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(WAR2_SRC))
KLAD1_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(KLAD1_SRC))

# Create directories for object files
OBJ_DIRS := $(sort $(dir $(WAR2_OBJ) $(KLAD1_OBJ)))

# Default target
all: $(BUILD_DIR) $(OBJ_DIRS) war2 klad1

# Linking step
war2: $(WAR2_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(WAR2_OBJ) $(LDFLAGS)

klad1: $(KLAD1_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(KLAD1_OBJ) $(LDFLAGS)

# Compilation step (separate object files in build/)
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR) $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build/ directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Ensure all required object directories exist
$(OBJ_DIRS):
	mkdir -p $@

# Clean command
clean:
	rm -rf war2 klad1 $(BUILD_DIR)
