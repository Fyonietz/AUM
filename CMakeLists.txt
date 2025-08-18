cmake_minimum_required(VERSION 3.10)
project(Phoenix)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

# Find Boost (Tokenizer is header-only)
find_package(Boost REQUIRED)

file(GLOB_RECURSE MAIN "main.cpp")
file(GLOB_RECURSE CIVET_C "libs/Civet/civetweb.c")
file(GLOB_RECURSE CIVET_INL "libs/Civet/*.inl")
file(GLOB_RECURSE CORE "core/*.cpp")
file(GLOB_RECURSE PAGES "pages/*.cpp")
FILE(GLOB_RECURSE ACT "act/*.cpp")

add_subdirectory(routes)

add_executable(${CMAKE_PROJECT_NAME} ${MAIN} ${CORE} ${CIVET_C} ${CIVET_INL} ${PAGES} ${ACT})

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE core
    PRIVATE pages
    PRIVATE routes
    PUBLIC libs
    PUBLIC /usr/include
    PUBLIC ${Boost_INCLUDE_DIRS}   # <-- Boost headers
)

add_definitions(-DNO_SSL)

target_link_libraries(${CMAKE_PROJECT_NAME}
    /usr/lib/x86_64-linux-gnu/libssl.so
    /usr/lib/x86_64-linux-gnu/libcrypto.so
    sqlite3
)

if (UNIX)
    target_link_libraries(${CMAKE_PROJECT_NAME} pthread dl)
endif()
