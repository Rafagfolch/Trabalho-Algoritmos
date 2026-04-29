cmake_minimum_required(VERSION 3.20)
project(exe C)

set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(exe main.c benchmarking.c)
target_link_libraries(exe ${CMAKE_SOURCE_DIR}/lib/libordenacao18.a)

# pra rodar no Wine e gerar o arquivo csv tem q ser
# x86_64-w64-mingw32-gcc main.c benchmarking.c -L./lib -lordenacao18 -o executavel.exe
# wine ./executavel.exe > trabalho.csv