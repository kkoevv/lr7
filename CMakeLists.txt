cmake_minimum_required(VERSION 3.16)
project(OOP7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

include_directories(${CMAKE_SOURCE_DIR}/include)

# Основная программа
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/npc.cpp
    src/Bear.cpp
    src/Werewolf.cpp
    src/Rogue.cpp
    src/GameManager.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Простые тесты
add_executable(simple_tests
    test/simple_tests.cpp
    src/npc.cpp
    src/Bear.cpp
    src/Werewolf.cpp
    src/Rogue.cpp
    src/GameManager.cpp
)

target_link_libraries(simple_tests PRIVATE Threads::Threads)