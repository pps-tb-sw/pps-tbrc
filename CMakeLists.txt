cmake_minimum_required(VERSION 2.6)
project(PPS_TB_RC)

set(CAEN_LOCATION "/usr/lib")
set(GCC_COMPILE_FLAGS "-Wall -fPIC -O2 -g -lsqlite3")
set(GCC_LINK_FLAGS "-lsqlite3")
set(QUSB_LOCATION "${PROJECT_SOURCE_DIR}/external/xpedaq/quickusb-2.15.2/")
set(QUSB_LIB_LOCATION "${QUSB_LOCATION}/linux/lib/x86_64/")

add_definitions(${GCC_COMPILE_FLAGS})

# File reader
file(GLOB reader_sources ${PROJECT_SOURCE_DIR}/src/File*.cpp)
add_library(reader_lib OBJECT ${reader_sources} ${tdc_sources})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(src_lib OBJECT ${sources})
set_property(TARGET src_lib PROPERTY LINK_FLAGS "-lsqlite3")

include_directories("${PROJECT_SOURCE_DIR}/daq/include")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${QUSB_LOCATION}")

# Here have DAQ
add_subdirectory(daq)

# Here have tests
add_subdirectory(test EXCLUDE_FROM_ALL)

# Here have DQM clients
add_subdirectory(dqm EXCLUDE_FROM_ALL)

add_executable(ppsRun main.cpp $<TARGET_OBJECTS:src_lib>)
set_property(TARGET ppsRun PROPERTY LINK_FLAGS "-lsqlite3")

add_executable(listener listener.cpp $<TARGET_OBJECTS:src_lib>)
set_property(TARGET listener PROPERTY LINK_FLAGS "-lsqlite3")

add_executable(ppsFetch fetch.cpp $<TARGET_OBJECTS:src_lib> $<TARGET_OBJECTS:daq_lib>)
set_property(TARGET ppsFetch PROPERTY LINK_FLAGS "-ltinyxml2 -lsqlite3 -L${QUSB_LIB_LOCATION} -lquickusb -Wl,-R${QUSB_LIB_LOCATION}")

# Copy the XML configuration files to the config/ folder
file(GLOB config_files RELATIVE ${PROJECT_SOURCE_DIR} config/*.xml)
foreach(_script ${config_files})
  configure_file(${_script} ${PROJECT_BINARY_DIR}/${_script} COPYONLY)
endforeach()
