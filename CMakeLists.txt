cmake_minimum_required(VERSION 3.15)
project(api_gateway)

set(CMAKE_CXX_STANDARD 17)

# Drogon and JSON dependencies
find_package(Drogon REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED)

# === Aeron system paths ===
set(AERON_INCLUDE_DIR /home/huzaifa/aeron/cppbuild/Release/include)
set(AERON_LIB_PATH /home/huzaifa/aeron/cppbuild/Release/lib/libaeron_client.a)

# === Include directories ===
include_directories(
    src
    ${AERON_INCLUDE_DIR}              # ✅ Aeron headers from system
    ${DROGON_INCLUDE_DIRS}            # ✅ Drogon headers
    external/concurrentqueue          # ✅ If used in your project
    external/sbe/my_app_messages      # ✅ Your SBE-generated headers
    /usr/local/include
)

# === Source files, excluding decoder.cpp ===
file(GLOB SRC_FILES src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder.cpp)

# === Executable ===
add_executable(api_gateway ${SRC_FILES})

# === Link libraries ===
target_link_libraries(api_gateway
    Drogon::Drogon
    /usr/local/lib/libaeronWrapper.a                       # ✅ System aeron wrapper library
    ${AERON_LIB_PATH}                                      # ✅ Direct path to Aeron .a file (after wrapper)
    pthread
    nlohmann_json::nlohmann_json
)

# === Optional: RPATH if you switch to shared libs ===
# set_target_properties(api_gateway PROPERTIES
#     BUILD_RPATH /home/huzaifa/aeron/cppbuild/Release/lib
# )
