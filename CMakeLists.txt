cmake_minimum_required(VERSION 3.25)
project(labwork_12_KatsuShooter)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

add_executable(labwork_12_KatsuShooter
        main.cpp)

target_link_libraries(labwork_12_KatsuShooter PUBLIC MyCoolDB)