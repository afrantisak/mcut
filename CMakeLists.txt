cmake_minimum_required (VERSION 2.6)
project (recorder)
add_executable(recorder receiver.cpp recorder.cpp)

ADD_DEFINITIONS("-std=c++0x") 

TARGET_LINK_LIBRARIES(recorder boost_system boost_program_options pthread)

