cmake_minimum_required(VERSION 3.22)
project(tema3pcom)


set(CMAKE_CXX_STANDARD 14)

add_executable(tema3pcom client.cpp client.h forms.h requests.cpp requests.h server_ops.cpp server_ops.h buffer.cpp buffer.h commands.cpp commands.h response_handlers.cpp response_handlers.h forms.cpp)
