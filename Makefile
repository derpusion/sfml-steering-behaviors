rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
src := $(call rwildcard,src/,*.cpp)

obj_dir := obj
obj = $(patsubst src/%.cpp,$(obj_dir)/%.o,$(src))

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

INTELMAC_INCLUDE=-I/usr/local/include                             # Intel mac
APPLESILICON_INCLUDE=-I/opt/homebrew/include                     # Apple Silicon
UBUNTU_APPLESILICON_INCLUDE=-I/usr/include                       # Apple Silicon Ubuntu VM
UBUNTU_INTEL_INCLUDE=-I/usr/include                              # Intel Ubuntu VM

INTELMAC_LIB=-L/usr/local/lib                                    # Intel mac
APPLESILICON_LIB=-L/opt/homebrew/lib                              # Apple Silicon
UBUNTU_APPLESILICON_LIB=-L/usr/lib/aarch64-linux-gnu             # Apple Silicon Ubuntu VM
UBUNTU_INTEL_LIB=-L/usr/lib/x86_64-linux-gnu                      # Intel Ubuntu VM

MACOS_INCLUDE=$(APPLESILICON_INCLUDE)
MACOS_LIB=$(APPLESILICON_LIB)
UBUNTU_INCLUDE=$(UBUNTU_APPLESILICON_INCLUDE)
UBUNTU_LIB=$(UBUNTU_APPLESILICON_LIB)

MACOS_COMPILER=/usr/bin/clang++
UBUNTU_COMPILER=/usr/bin/g++

.PHONY: all clean

all: part1 part2 part3 part4

$(obj_dir)/%.o: src/headers/%.cpp
	$(UBUNTU_COMPILER) -std=c++17 -c $< -o $(obj_dir)/$(notdir $@) $(UBUNTU_INCLUDE) $(UBUNTU_LIB)

$(obj_dir)/%.o: src/main/%.cpp
	$(UBUNTU_COMPILER) -std=c++17 -c $< -o $(obj_dir)/$(notdir $@) $(UBUNTU_INCLUDE) $(UBUNTU_LIB)

part1: $(obj_dir)/steering.o $(obj_dir)/boid.o $(obj_dir)/velocityMatching.o
	$(UBUNTU_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)

part2: $(obj_dir)/steering.o $(obj_dir)/boid.o $(obj_dir)/positionMatching.o
	$(UBUNTU_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)

part3: $(obj_dir)/steering.o $(obj_dir)/boid.o $(obj_dir)/wander.o $(obj_dir)/wanderMain.o
	$(UBUNTU_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)

part4: $(obj_dir)/steering.o $(obj_dir)/boid.o $(obj_dir)/wander.o $(obj_dir)/flocking.o
	$(UBUNTU_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)

clean:
	find $(obj_dir) -type f ! -name 'README.md' -delete
	rm -f part1 part2 part3 part4