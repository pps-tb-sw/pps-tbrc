file(GLOB fpga_sources src/*Handler.cpp)

#add_library(DAQ SHARED ${fpga_sources} ${tdc_sources})
#set_property(TARGET DAQ PROPERTY LINK_FLAGS "-L${QUSB_LIB_LOCATION} -lquickusb -Wl,-R${QUSB_LIB_LOCATION}")
#include_directories("${PROJECT_SOURCE_DIR}/mblib")
#message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
#add_subdirectory("${PROJECT_SOURCE_DIR} mblib")
link_directories("${PROJECT_SOURCE_DIR}/mblib")

add_library(daq_lib OBJECT ${fpga_sources})
set_property(TARGET daq_lib PROPERTY LINK_FLAGS "-ltinyxml2 -lsqlite3 -L${QUSB_LIB_LOCATION} -lquickusb -Wl,-R${QUSB_LIB_LOCATION}")
#target_link_libraries(daq_lib $<TARGET_OBJECTS:mb_lib>)
