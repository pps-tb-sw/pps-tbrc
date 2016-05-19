file(GLOB mb_sources mblib/src/TDC*.cpp)
include_directories(mblib/include)
add_library(mb_lib SHARED ${mb_sources})
set_property(TARGET mb_lib PROPERTY LINK_FLAGS "-L${QUSB_LIB_LOCATION} -lquickusb -Wl,-R${QUSB_LIB_LOCATION}")
