cmake_minimum_required(VERSION 3.15)
project(api_gateway)

set(CMAKE_CXX_STANDARD 17)

find_package(Drogon REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED)
find_package(aeronWrapper CONFIG REQUIRED)

include_directories(
    src
    external/concurrentqueue          
    external/sbe/SBE      
    /usr/local/include
    external/eloan-main/include/eloan
    external/eloan-main/include       
)

# === Source files, excluding decoder.cpp ===
file(GLOB SRC_FILES src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder.cpp)

# Add ShardedQueue implementation
list(APPEND SRC_FILES external/eloan-main/src/sharded_queue.cpp)

# === Executable ===
add_executable(api_gateway ${SRC_FILES})

# === Link libraries ===
target_link_libraries(api_gateway
    Drogon::Drogon
    aeronWrapper::aeronWrapper
    pthread
    nlohmann_json::nlohmann_json
)