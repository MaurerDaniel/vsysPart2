cmake_minimum_required(VERSION 3.12)
project(vsys_email_server)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Server_Data)

add_executable(vsys_email_server
        Server_Data/asoiuoiuoiuiou/currentID.txt
        Server_Data/astrid/currentID.txt
        Server_Data/aösdlkfjaösldkjfaöslkdjföalskdjfölaskdjf/currentID.txt
        Server_Data/maximilian/currentID.txt
        Server_Data/maximilianwist/currentID.txt
        Server_Data/neu/currentID.txt
        Server_Data/neuerTestUseradsf/currentID.txt
        Server_Data/receiver/currentID.txt
        Server_Data/receiver\n/currentID.txt
        Server_Data/receiver123/currentID.txt
        Server_Data/receiver988888/currentID.txt
        Server_Data/SEND/currentID.txt
        ClientHandler.cpp
        ClientHandler.h
        FileHandler.cpp
        FileHandler.h
        Functions.cpp
        Functions.h
        main.cpp
        Makefile
        MessageType.h)
