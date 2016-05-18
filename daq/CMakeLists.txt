file(GLOB fpga_sources src/*Handler.cpp)
file(GLOB tdc_sources src/TDC*.cpp)

#add_library(DAQ SHARED ${fpga_sources} ${tdc_sources})
#set_property(TARGET DAQ PROPERTY LINK_FLAGS "-L${QUSB_LIB_LOCATION} -lquickusb -Wl,-R${QUSB_LIB_LOCATION}")

add_library(daq_lib OBJECT ${fpga_sources} ${tdc_sources})
set_property(TARGET daq_lib PROPERTY LINK_FLAGS "-ltinyxml2 -lsqlite3 -L${QUSB_LIB_LOCATION} -lquickusb -Wl,-R${QUSB_LIB_LOCATION}")
