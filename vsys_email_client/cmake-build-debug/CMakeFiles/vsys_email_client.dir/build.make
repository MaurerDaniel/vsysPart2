# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /home/user/Software/clion-2018.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/user/Software/clion-2018.2.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/user/Documents/VSYS/vsys_email_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/Documents/VSYS/vsys_email_client/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/vsys_email_client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vsys_email_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vsys_email_client.dir/flags.make

CMakeFiles/vsys_email_client.dir/main.cpp.o: CMakeFiles/vsys_email_client.dir/flags.make
CMakeFiles/vsys_email_client.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Documents/VSYS/vsys_email_client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/vsys_email_client.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vsys_email_client.dir/main.cpp.o -c /home/user/Documents/VSYS/vsys_email_client/main.cpp

CMakeFiles/vsys_email_client.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vsys_email_client.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Documents/VSYS/vsys_email_client/main.cpp > CMakeFiles/vsys_email_client.dir/main.cpp.i

CMakeFiles/vsys_email_client.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vsys_email_client.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Documents/VSYS/vsys_email_client/main.cpp -o CMakeFiles/vsys_email_client.dir/main.cpp.s

CMakeFiles/vsys_email_client.dir/protocol.cpp.o: CMakeFiles/vsys_email_client.dir/flags.make
CMakeFiles/vsys_email_client.dir/protocol.cpp.o: ../protocol.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Documents/VSYS/vsys_email_client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/vsys_email_client.dir/protocol.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vsys_email_client.dir/protocol.cpp.o -c /home/user/Documents/VSYS/vsys_email_client/protocol.cpp

CMakeFiles/vsys_email_client.dir/protocol.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vsys_email_client.dir/protocol.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Documents/VSYS/vsys_email_client/protocol.cpp > CMakeFiles/vsys_email_client.dir/protocol.cpp.i

CMakeFiles/vsys_email_client.dir/protocol.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vsys_email_client.dir/protocol.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Documents/VSYS/vsys_email_client/protocol.cpp -o CMakeFiles/vsys_email_client.dir/protocol.cpp.s

CMakeFiles/vsys_email_client.dir/Functions.cpp.o: CMakeFiles/vsys_email_client.dir/flags.make
CMakeFiles/vsys_email_client.dir/Functions.cpp.o: ../Functions.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Documents/VSYS/vsys_email_client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/vsys_email_client.dir/Functions.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vsys_email_client.dir/Functions.cpp.o -c /home/user/Documents/VSYS/vsys_email_client/Functions.cpp

CMakeFiles/vsys_email_client.dir/Functions.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vsys_email_client.dir/Functions.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Documents/VSYS/vsys_email_client/Functions.cpp > CMakeFiles/vsys_email_client.dir/Functions.cpp.i

CMakeFiles/vsys_email_client.dir/Functions.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vsys_email_client.dir/Functions.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Documents/VSYS/vsys_email_client/Functions.cpp -o CMakeFiles/vsys_email_client.dir/Functions.cpp.s

# Object files for target vsys_email_client
vsys_email_client_OBJECTS = \
"CMakeFiles/vsys_email_client.dir/main.cpp.o" \
"CMakeFiles/vsys_email_client.dir/protocol.cpp.o" \
"CMakeFiles/vsys_email_client.dir/Functions.cpp.o"

# External object files for target vsys_email_client
vsys_email_client_EXTERNAL_OBJECTS =

vsys_email_client: CMakeFiles/vsys_email_client.dir/main.cpp.o
vsys_email_client: CMakeFiles/vsys_email_client.dir/protocol.cpp.o
vsys_email_client: CMakeFiles/vsys_email_client.dir/Functions.cpp.o
vsys_email_client: CMakeFiles/vsys_email_client.dir/build.make
vsys_email_client: CMakeFiles/vsys_email_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/Documents/VSYS/vsys_email_client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable vsys_email_client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vsys_email_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vsys_email_client.dir/build: vsys_email_client

.PHONY : CMakeFiles/vsys_email_client.dir/build

CMakeFiles/vsys_email_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vsys_email_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vsys_email_client.dir/clean

CMakeFiles/vsys_email_client.dir/depend:
	cd /home/user/Documents/VSYS/vsys_email_client/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/Documents/VSYS/vsys_email_client /home/user/Documents/VSYS/vsys_email_client /home/user/Documents/VSYS/vsys_email_client/cmake-build-debug /home/user/Documents/VSYS/vsys_email_client/cmake-build-debug /home/user/Documents/VSYS/vsys_email_client/cmake-build-debug/CMakeFiles/vsys_email_client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vsys_email_client.dir/depend
