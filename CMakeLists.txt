cmake_minimum_required(VERSION 3.19)
project(e_goat C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-Wall -pedantic -std=gnu99")

add_executable(client client.c)
add_executable(server server.c)