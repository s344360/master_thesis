# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /opt/clion-2020.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2020.2.4/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/seadmin/keyless_entry

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/seadmin/keyless_entry/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/door_modules.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/door_modules.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/door_modules.dir/flags.make

CMakeFiles/door_modules.dir/door_modules.cpp.o: CMakeFiles/door_modules.dir/flags.make
CMakeFiles/door_modules.dir/door_modules.cpp.o: ../door_modules.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/seadmin/keyless_entry/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/door_modules.dir/door_modules.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/door_modules.dir/door_modules.cpp.o -c /home/seadmin/keyless_entry/door_modules.cpp

CMakeFiles/door_modules.dir/door_modules.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/door_modules.dir/door_modules.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/seadmin/keyless_entry/door_modules.cpp > CMakeFiles/door_modules.dir/door_modules.cpp.i

CMakeFiles/door_modules.dir/door_modules.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/door_modules.dir/door_modules.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/seadmin/keyless_entry/door_modules.cpp -o CMakeFiles/door_modules.dir/door_modules.cpp.s

# Object files for target door_modules
door_modules_OBJECTS = \
"CMakeFiles/door_modules.dir/door_modules.cpp.o"

# External object files for target door_modules
door_modules_EXTERNAL_OBJECTS =

door_modules: CMakeFiles/door_modules.dir/door_modules.cpp.o
door_modules: CMakeFiles/door_modules.dir/build.make
door_modules: ../library/libopen62541.a
door_modules: CMakeFiles/door_modules.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/seadmin/keyless_entry/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable door_modules"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/door_modules.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/door_modules.dir/build: door_modules

.PHONY : CMakeFiles/door_modules.dir/build

CMakeFiles/door_modules.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/door_modules.dir/cmake_clean.cmake
.PHONY : CMakeFiles/door_modules.dir/clean

CMakeFiles/door_modules.dir/depend:
	cd /home/seadmin/keyless_entry/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/seadmin/keyless_entry /home/seadmin/keyless_entry /home/seadmin/keyless_entry/cmake-build-debug /home/seadmin/keyless_entry/cmake-build-debug /home/seadmin/keyless_entry/cmake-build-debug/CMakeFiles/door_modules.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/door_modules.dir/depend
