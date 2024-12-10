# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/strcarne/Documents/qt-based/string-searcher/ngrams

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/strcarne/Documents/qt-based/string-searcher/ngrams/build

# Include any dependencies generated for this target.
include CMakeFiles/ngrams.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ngrams.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ngrams.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ngrams.dir/flags.make

CMakeFiles/ngrams.dir/src/engine.cpp.o: CMakeFiles/ngrams.dir/flags.make
CMakeFiles/ngrams.dir/src/engine.cpp.o: /home/strcarne/Documents/qt-based/string-searcher/ngrams/src/engine.cpp
CMakeFiles/ngrams.dir/src/engine.cpp.o: CMakeFiles/ngrams.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/strcarne/Documents/qt-based/string-searcher/ngrams/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ngrams.dir/src/engine.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ngrams.dir/src/engine.cpp.o -MF CMakeFiles/ngrams.dir/src/engine.cpp.o.d -o CMakeFiles/ngrams.dir/src/engine.cpp.o -c /home/strcarne/Documents/qt-based/string-searcher/ngrams/src/engine.cpp

CMakeFiles/ngrams.dir/src/engine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ngrams.dir/src/engine.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/strcarne/Documents/qt-based/string-searcher/ngrams/src/engine.cpp > CMakeFiles/ngrams.dir/src/engine.cpp.i

CMakeFiles/ngrams.dir/src/engine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ngrams.dir/src/engine.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/strcarne/Documents/qt-based/string-searcher/ngrams/src/engine.cpp -o CMakeFiles/ngrams.dir/src/engine.cpp.s

# Object files for target ngrams
ngrams_OBJECTS = \
"CMakeFiles/ngrams.dir/src/engine.cpp.o"

# External object files for target ngrams
ngrams_EXTERNAL_OBJECTS =

libngrams.a: CMakeFiles/ngrams.dir/src/engine.cpp.o
libngrams.a: CMakeFiles/ngrams.dir/build.make
libngrams.a: CMakeFiles/ngrams.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/strcarne/Documents/qt-based/string-searcher/ngrams/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libngrams.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ngrams.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ngrams.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ngrams.dir/build: libngrams.a
.PHONY : CMakeFiles/ngrams.dir/build

CMakeFiles/ngrams.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ngrams.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ngrams.dir/clean

CMakeFiles/ngrams.dir/depend:
	cd /home/strcarne/Documents/qt-based/string-searcher/ngrams/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/strcarne/Documents/qt-based/string-searcher/ngrams /home/strcarne/Documents/qt-based/string-searcher/ngrams /home/strcarne/Documents/qt-based/string-searcher/ngrams/build /home/strcarne/Documents/qt-based/string-searcher/ngrams/build /home/strcarne/Documents/qt-based/string-searcher/ngrams/build/CMakeFiles/ngrams.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ngrams.dir/depend
