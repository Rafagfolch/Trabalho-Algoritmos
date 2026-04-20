cmake_minimum_required(VERSION 3.20)
project(exe C)

set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(exe main.c)
target_link_libraries(exe ${CMAKE_SOURCE_DIR}/lib/libordenacao18.a)

# pra rodar tem q ser 
# gcc main.c -L./lib -lordenacao18 -o exe