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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kokabi/git/pps-tbrc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kokabi/git/pps-tbrc

# Include any dependencies generated for this target.
include CMakeFiles/ppsFetch.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ppsFetch.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ppsFetch.dir/flags.make

CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o: CMakeFiles/ppsFetch.dir/flags.make
CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o: fetch_vme.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kokabi/git/pps-tbrc/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o -c /home/kokabi/git/pps-tbrc/fetch_vme.cpp

CMakeFiles/ppsFetch.dir/fetch_vme.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ppsFetch.dir/fetch_vme.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kokabi/git/pps-tbrc/fetch_vme.cpp > CMakeFiles/ppsFetch.dir/fetch_vme.cpp.i

CMakeFiles/ppsFetch.dir/fetch_vme.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ppsFetch.dir/fetch_vme.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kokabi/git/pps-tbrc/fetch_vme.cpp -o CMakeFiles/ppsFetch.dir/fetch_vme.cpp.s

CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.requires:
.PHONY : CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.requires

CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.provides: CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.requires
	$(MAKE) -f CMakeFiles/ppsFetch.dir/build.make CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.provides.build
.PHONY : CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.provides

CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.provides.build: CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o

# Object files for target ppsFetch
ppsFetch_OBJECTS = \
"CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o"

# External object files for target ppsFetch
ppsFetch_EXTERNAL_OBJECTS = \
"/home/kokabi/git/pps-tbrc/CMakeFiles/src_lib.dir/src/Socket.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/src_lib.dir/src/Client.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/src_lib.dir/src/Messenger.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VME_CFDV812.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VME_IOModuleV262.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VMEReader.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VME_FPGAUnitV1495.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VME_CAENETControllerV288.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VME_TDCV1x90.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/VME_BridgeVx718.cpp.o" \
"/home/kokabi/git/pps-tbrc/CMakeFiles/det_lib.dir/src/NIM_HVModuleN470.cpp.o" \
"/home/kokabi/git/pps-tbrc/external/CMakeFiles/ws_lib.dir/WebSocket/WebSocket.cpp.o" \
"/home/kokabi/git/pps-tbrc/external/CMakeFiles/ws_lib.dir/WebSocket/sha1/sha1.cpp.o" \
"/home/kokabi/git/pps-tbrc/external/CMakeFiles/ws_lib.dir/WebSocket/base64/base64.cpp.o"

ppsFetch: CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o
ppsFetch: CMakeFiles/src_lib.dir/src/Socket.cpp.o
ppsFetch: CMakeFiles/src_lib.dir/src/Client.cpp.o
ppsFetch: CMakeFiles/src_lib.dir/src/Messenger.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VME_CFDV812.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VME_IOModuleV262.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VMEReader.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VME_FPGAUnitV1495.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VME_CAENETControllerV288.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VME_TDCV1x90.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/VME_BridgeVx718.cpp.o
ppsFetch: CMakeFiles/det_lib.dir/src/NIM_HVModuleN470.cpp.o
ppsFetch: external/CMakeFiles/ws_lib.dir/WebSocket/WebSocket.cpp.o
ppsFetch: external/CMakeFiles/ws_lib.dir/WebSocket/sha1/sha1.cpp.o
ppsFetch: external/CMakeFiles/ws_lib.dir/WebSocket/base64/base64.cpp.o
ppsFetch: CMakeFiles/ppsFetch.dir/build.make
ppsFetch: /usr/lib/libCAENVME.so
ppsFetch: CMakeFiles/ppsFetch.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ppsFetch"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ppsFetch.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ppsFetch.dir/build: ppsFetch
.PHONY : CMakeFiles/ppsFetch.dir/build

CMakeFiles/ppsFetch.dir/requires: CMakeFiles/ppsFetch.dir/fetch_vme.cpp.o.requires
.PHONY : CMakeFiles/ppsFetch.dir/requires

CMakeFiles/ppsFetch.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ppsFetch.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ppsFetch.dir/clean

CMakeFiles/ppsFetch.dir/depend:
	cd /home/kokabi/git/pps-tbrc && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kokabi/git/pps-tbrc /home/kokabi/git/pps-tbrc /home/kokabi/git/pps-tbrc /home/kokabi/git/pps-tbrc /home/kokabi/git/pps-tbrc/CMakeFiles/ppsFetch.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ppsFetch.dir/depend
