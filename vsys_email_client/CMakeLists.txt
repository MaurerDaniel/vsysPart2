cmake_minimum_required(VERSION 3.9)
project(vsys_email_client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(vsys_email_client main.cpp protocol.cpp protocol.h Functions.cpp Functions.h)