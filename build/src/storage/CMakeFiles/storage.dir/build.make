# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/albert/rucbase-lab

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/albert/rucbase-lab/build

# Include any dependencies generated for this target.
include src/storage/CMakeFiles/storage.dir/depend.make

# Include the progress variables for this target.
include src/storage/CMakeFiles/storage.dir/progress.make

# Include the compile flags for this target's objects.
include src/storage/CMakeFiles/storage.dir/flags.make

src/storage/CMakeFiles/storage.dir/disk_manager.cpp.o: src/storage/CMakeFiles/storage.dir/flags.make
src/storage/CMakeFiles/storage.dir/disk_manager.cpp.o: ../src/storage/disk_manager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/albert/rucbase-lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/storage/CMakeFiles/storage.dir/disk_manager.cpp.o"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/storage.dir/disk_manager.cpp.o -c /home/albert/rucbase-lab/src/storage/disk_manager.cpp

src/storage/CMakeFiles/storage.dir/disk_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/storage.dir/disk_manager.cpp.i"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/albert/rucbase-lab/src/storage/disk_manager.cpp > CMakeFiles/storage.dir/disk_manager.cpp.i

src/storage/CMakeFiles/storage.dir/disk_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/storage.dir/disk_manager.cpp.s"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/albert/rucbase-lab/src/storage/disk_manager.cpp -o CMakeFiles/storage.dir/disk_manager.cpp.s

src/storage/CMakeFiles/storage.dir/buffer_pool_manager.cpp.o: src/storage/CMakeFiles/storage.dir/flags.make
src/storage/CMakeFiles/storage.dir/buffer_pool_manager.cpp.o: ../src/storage/buffer_pool_manager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/albert/rucbase-lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/storage/CMakeFiles/storage.dir/buffer_pool_manager.cpp.o"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/storage.dir/buffer_pool_manager.cpp.o -c /home/albert/rucbase-lab/src/storage/buffer_pool_manager.cpp

src/storage/CMakeFiles/storage.dir/buffer_pool_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/storage.dir/buffer_pool_manager.cpp.i"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/albert/rucbase-lab/src/storage/buffer_pool_manager.cpp > CMakeFiles/storage.dir/buffer_pool_manager.cpp.i

src/storage/CMakeFiles/storage.dir/buffer_pool_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/storage.dir/buffer_pool_manager.cpp.s"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/albert/rucbase-lab/src/storage/buffer_pool_manager.cpp -o CMakeFiles/storage.dir/buffer_pool_manager.cpp.s

src/storage/CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.o: src/storage/CMakeFiles/storage.dir/flags.make
src/storage/CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.o: ../src/replacer/lru_replacer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/albert/rucbase-lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/storage/CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.o"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.o -c /home/albert/rucbase-lab/src/replacer/lru_replacer.cpp

src/storage/CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.i"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/albert/rucbase-lab/src/replacer/lru_replacer.cpp > CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.i

src/storage/CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.s"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/albert/rucbase-lab/src/replacer/lru_replacer.cpp -o CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.s

src/storage/CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.o: src/storage/CMakeFiles/storage.dir/flags.make
src/storage/CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.o: ../src/replacer/clock_replacer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/albert/rucbase-lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/storage/CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.o"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.o -c /home/albert/rucbase-lab/src/replacer/clock_replacer.cpp

src/storage/CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.i"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/albert/rucbase-lab/src/replacer/clock_replacer.cpp > CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.i

src/storage/CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.s"
	cd /home/albert/rucbase-lab/build/src/storage && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/albert/rucbase-lab/src/replacer/clock_replacer.cpp -o CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.s

# Object files for target storage
storage_OBJECTS = \
"CMakeFiles/storage.dir/disk_manager.cpp.o" \
"CMakeFiles/storage.dir/buffer_pool_manager.cpp.o" \
"CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.o" \
"CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.o"

# External object files for target storage
storage_EXTERNAL_OBJECTS =

lib/libstorage.a: src/storage/CMakeFiles/storage.dir/disk_manager.cpp.o
lib/libstorage.a: src/storage/CMakeFiles/storage.dir/buffer_pool_manager.cpp.o
lib/libstorage.a: src/storage/CMakeFiles/storage.dir/__/replacer/lru_replacer.cpp.o
lib/libstorage.a: src/storage/CMakeFiles/storage.dir/__/replacer/clock_replacer.cpp.o
lib/libstorage.a: src/storage/CMakeFiles/storage.dir/build.make
lib/libstorage.a: src/storage/CMakeFiles/storage.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/albert/rucbase-lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX static library ../../lib/libstorage.a"
	cd /home/albert/rucbase-lab/build/src/storage && $(CMAKE_COMMAND) -P CMakeFiles/storage.dir/cmake_clean_target.cmake
	cd /home/albert/rucbase-lab/build/src/storage && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/storage.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/storage/CMakeFiles/storage.dir/build: lib/libstorage.a

.PHONY : src/storage/CMakeFiles/storage.dir/build

src/storage/CMakeFiles/storage.dir/clean:
	cd /home/albert/rucbase-lab/build/src/storage && $(CMAKE_COMMAND) -P CMakeFiles/storage.dir/cmake_clean.cmake
.PHONY : src/storage/CMakeFiles/storage.dir/clean

src/storage/CMakeFiles/storage.dir/depend:
	cd /home/albert/rucbase-lab/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/albert/rucbase-lab /home/albert/rucbase-lab/src/storage /home/albert/rucbase-lab/build /home/albert/rucbase-lab/build/src/storage /home/albert/rucbase-lab/build/src/storage/CMakeFiles/storage.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/storage/CMakeFiles/storage.dir/depend

