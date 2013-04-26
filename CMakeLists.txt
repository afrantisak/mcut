cmake_minimum_required(VERSION 2.6)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(mcut)

add_executable(mc-echo mc-echo.cpp receiver.cpp recorder.cpp ArgParser.cpp)

enable_testing()

ADD_DEFINITIONS("-std=c++0x") 

TARGET_LINK_LIBRARIES(mc-echo boost_system boost_program_options pthread rt)

macro(add_test_diff name file command)
  add_test(${name} python cmdiff.py ${file} ${command})
endmacro(add_test_diff)

add_test(help bin/mc-echo --help)
add_test_diff(help_message test/help/test.out "bin/mc-echo --help")
add_test_diff(mcast_stdout test/mcast_stdout/test.ref "python test/mcast_stdout/test.py")
