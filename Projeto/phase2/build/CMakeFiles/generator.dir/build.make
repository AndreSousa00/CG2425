# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build"

# Include any dependencies generated for this target.
include CMakeFiles/generator.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/generator.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/generator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/generator.dir/flags.make

CMakeFiles/generator.dir/codegen:
.PHONY : CMakeFiles/generator.dir/codegen

CMakeFiles/generator.dir/generator.cpp.o: CMakeFiles/generator.dir/flags.make
CMakeFiles/generator.dir/generator.cpp.o: /Users/jnf/Documents/University/2024_2025/Second\ Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/generator.cpp
CMakeFiles/generator.dir/generator.cpp.o: CMakeFiles/generator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/generator.dir/generator.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/generator.dir/generator.cpp.o -MF CMakeFiles/generator.dir/generator.cpp.o.d -o CMakeFiles/generator.dir/generator.cpp.o -c "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/generator.cpp"

CMakeFiles/generator.dir/generator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/generator.dir/generator.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/generator.cpp" > CMakeFiles/generator.dir/generator.cpp.i

CMakeFiles/generator.dir/generator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/generator.dir/generator.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/generator.cpp" -o CMakeFiles/generator.dir/generator.cpp.s

# Object files for target generator
generator_OBJECTS = \
"CMakeFiles/generator.dir/generator.cpp.o"

# External object files for target generator
generator_EXTERNAL_OBJECTS =

generator: CMakeFiles/generator.dir/generator.cpp.o
generator: CMakeFiles/generator.dir/build.make
generator: CMakeFiles/generator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable generator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/generator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/generator.dir/build: generator
.PHONY : CMakeFiles/generator.dir/build

CMakeFiles/generator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/generator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/generator.dir/clean

CMakeFiles/generator.dir/depend:
	cd "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2" "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2" "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build" "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build" "/Users/jnf/Documents/University/2024_2025/Second Semester/CG/TrabalhoPratico/Git/CG2425/Projeto/phase2/build/CMakeFiles/generator.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/generator.dir/depend

