cmake_minimum_required(VERSION 3.15)
project(api_gateway)

set(CMAKE_CXX_STANDARD 17)

# Drogon and JSON dependencies
find_package(Drogon REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED)
find_package(aeronWrapper CONFIG REQUIRED)

# === Aeron system paths ===
# set(AERON_SOURCE_DIR /home/it-admin/aeron-sbe-setup/aeron/aeron-client/src/main/cpp)
# set(AERON_BUILD_DIR /home/it-admin/aeron-sbe-setup/aeron/build)
# set(AERON_LIB_PATH ${AERON_BUILD_DIR}/lib/libaeron_client.a)

# === Include directories ===
include_directories(
    src
    # ${AERON_SOURCE_DIR}               # ✅ Aeron headers from source directory
    # ${DROGON_INCLUDE_DIRS}            # ✅ Drogon headers

    external/concurrentqueue          # ✅ If used in your project
    external/sbe/my_app_messages      # ✅ Your SBE-generated headers
    /usr/local/include
    external/eloan-main/include/eloan
    external/eloan-main/include       # Add eloan include directory
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
    # /usr/local/lib/libaeronWrapper.a                # ✅ System aeron wrapper library
    aeronWrapper::aeronWrapper
    # ${AERON_LIB_PATH}                                      # ✅ Direct path to Aeron .a file (after wrapper)
    pthread
    nlohmann_json::nlohmann_json
)