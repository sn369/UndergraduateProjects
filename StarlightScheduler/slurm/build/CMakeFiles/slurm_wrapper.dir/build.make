# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/workspace/src/starlight-scheduler/slurm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/workspace/src/starlight-scheduler/slurm/build

# Include any dependencies generated for this target.
include CMakeFiles/slurm_wrapper.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/slurm_wrapper.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/slurm_wrapper.dir/flags.make

CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o: CMakeFiles/slurm_wrapper.dir/flags.make
CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o: ../src/slurm_wrapper.c
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/src/starlight-scheduler/slurm/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o   -c /root/workspace/src/starlight-scheduler/slurm/src/slurm_wrapper.c

CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /root/workspace/src/starlight-scheduler/slurm/src/slurm_wrapper.c > CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.i

CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /root/workspace/src/starlight-scheduler/slurm/src/slurm_wrapper.c -o CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.s

CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.requires:
.PHONY : CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.requires

CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.provides: CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.requires
	$(MAKE) -f CMakeFiles/slurm_wrapper.dir/build.make CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.provides.build
.PHONY : CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.provides

CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.provides.build: CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o

# Object files for target slurm_wrapper
slurm_wrapper_OBJECTS = \
"CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o"

# External object files for target slurm_wrapper
slurm_wrapper_EXTERNAL_OBJECTS =

../lib64/libslurm_wrapper.a: CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o
../lib64/libslurm_wrapper.a: CMakeFiles/slurm_wrapper.dir/build.make
../lib64/libslurm_wrapper.a: CMakeFiles/slurm_wrapper.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library ../lib64/libslurm_wrapper.a"
	$(CMAKE_COMMAND) -P CMakeFiles/slurm_wrapper.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/slurm_wrapper.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/slurm_wrapper.dir/build: ../lib64/libslurm_wrapper.a
.PHONY : CMakeFiles/slurm_wrapper.dir/build

CMakeFiles/slurm_wrapper.dir/requires: CMakeFiles/slurm_wrapper.dir/src/slurm_wrapper.c.o.requires
.PHONY : CMakeFiles/slurm_wrapper.dir/requires

CMakeFiles/slurm_wrapper.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/slurm_wrapper.dir/cmake_clean.cmake
.PHONY : CMakeFiles/slurm_wrapper.dir/clean

CMakeFiles/slurm_wrapper.dir/depend:
	cd /root/workspace/src/starlight-scheduler/slurm/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/src/starlight-scheduler/slurm /root/workspace/src/starlight-scheduler/slurm /root/workspace/src/starlight-scheduler/slurm/build /root/workspace/src/starlight-scheduler/slurm/build /root/workspace/src/starlight-scheduler/slurm/build/CMakeFiles/slurm_wrapper.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/slurm_wrapper.dir/depend

