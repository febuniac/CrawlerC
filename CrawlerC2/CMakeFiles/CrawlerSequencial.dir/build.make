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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.12.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.12.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/felipebuniac/Github/CrawlerC/P2igor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/felipebuniac/Github/CrawlerC/P2igor

# Include any dependencies generated for this target.
include CMakeFiles/CrawlerSequencial.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CrawlerSequencial.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CrawlerSequencial.dir/flags.make

CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.o: CMakeFiles/CrawlerSequencial.dir/flags.make
CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.o: crawler_seq3.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felipebuniac/Github/CrawlerC/P2igor/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.o -c /Users/felipebuniac/Github/CrawlerC/P2igor/crawler_seq3.cpp

CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felipebuniac/Github/CrawlerC/P2igor/crawler_seq3.cpp > CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.i

CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felipebuniac/Github/CrawlerC/P2igor/crawler_seq3.cpp -o CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.s

# Object files for target CrawlerSequencial
CrawlerSequencial_OBJECTS = \
"CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.o"

# External object files for target CrawlerSequencial
CrawlerSequencial_EXTERNAL_OBJECTS =

CrawlerSequencial: CMakeFiles/CrawlerSequencial.dir/crawler_seq3.cpp.o
CrawlerSequencial: CMakeFiles/CrawlerSequencial.dir/build.make
CrawlerSequencial: CMakeFiles/CrawlerSequencial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/felipebuniac/Github/CrawlerC/P2igor/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable CrawlerSequencial"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CrawlerSequencial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CrawlerSequencial.dir/build: CrawlerSequencial

.PHONY : CMakeFiles/CrawlerSequencial.dir/build

CMakeFiles/CrawlerSequencial.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CrawlerSequencial.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CrawlerSequencial.dir/clean

CMakeFiles/CrawlerSequencial.dir/depend:
	cd /Users/felipebuniac/Github/CrawlerC/P2igor && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/felipebuniac/Github/CrawlerC/P2igor /Users/felipebuniac/Github/CrawlerC/P2igor /Users/felipebuniac/Github/CrawlerC/P2igor /Users/felipebuniac/Github/CrawlerC/P2igor /Users/felipebuniac/Github/CrawlerC/P2igor/CMakeFiles/CrawlerSequencial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CrawlerSequencial.dir/depend

