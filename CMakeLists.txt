cmake_minimum_required(VERSION 2.6)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(mcut)

# mc-recv-raw
add_executable(mc-recv-raw mc-recv-raw.cpp receiver.cpp Source.cpp ArgParser.cpp ConsoleInterrupt.cpp Packet.cpp)
TARGET_LINK_LIBRARIES(mc-recv-raw boost_system boost_program_options pthread rt)

# mc-recv-pkt
add_executable(mc-recv-pkt mc-recv-pkt.cpp receiver.cpp Source.cpp ArgParser.cpp ConsoleInterrupt.cpp Packet.cpp)
TARGET_LINK_LIBRARIES(mc-recv-pkt boost_system boost_program_options pthread rt)

# largeFileWriteTest
add_executable(largeFileWriteTest largeFileWriteTest.cpp ArgParser.cpp ConsoleInterrupt.cpp Packet.cpp)
TARGET_LINK_LIBRARIES(largeFileWriteTest boost_system boost_program_options boost_iostreams pthread rt)

# boostex
#add_executable(boostex boostex.cpp)
#TARGET_LINK_LIBRARIES(boostex boost_iostreams)

enable_testing()

ADD_DEFINITIONS("-std=c++0x") 


macro(add_test_diff name file command)
  add_test(${name} python cmdiff.py ${file} ${command})
endmacro(add_test_diff)

add_test(help bin/mc-recv-raw --help)
add_test_diff(help_message test/help/test.out "bin/mc-recv-raw --help")
add_test_diff(mcast_stdout test/mcast_stdout/test.ref "python test/mcast_stdout/test.py")


#cmake_minimum_required(VERSION 2.8)
#find_package(Boost COMPONENTS iostreams REQUIRED)
# 
#add_executable(memory-map main.cpp)
#target_link_libraries(memory-map ${Boost_LIBRARIES})