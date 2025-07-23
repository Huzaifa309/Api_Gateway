cmake_minimum_required(VERSION 3.15)
project(api_gateway)

set(CMAKE_CXX_STANDARD 17)

# Drogon dependencies
find_package(Drogon REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED)

include(FetchContent)
FetchContent_Declare(
    aeron
    GIT_REPOSITORY https://github.com/aeron-io/aeron.git
    GIT_TAG        master  # or specify a specific version tag like "1.45.0"
)





set(BUILD_AERON_DRIVER ON CACHE BOOL "Build Aeron driver")
set(BUILD_AERON_ARCHIVE_API ON CACHE BOOL "Build Aeron archive API")
set(C_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors")
set(CXX_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors")
# Aeron paths - Updated to point to the actual Aeron installation
set(AERON_SOURCE_DIR /home/it-admin/aeron-sbe-setup/aeron/aeron-client/src/main/cpp)
set(AERON_BUILD_DIR /home/it-admin/aeron-sbe-setup/aeron/build)


FetchContent_MakeAvailable(aeron)

# Includes
include_directories(
    src
    ${AERON_SOURCE_DIR}             # Aeron C++ headers
    ${DROGON_INCLUDE_DIRS}
    external/concurrentqueue
    external/sbe/my_app_messages
    external/wrapper
    aeron/aeron-client/src/main/cpp
)

# All source files except decoder.cpp (which is a separate standalone program)
file(GLOB SRC_FILES src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder.cpp)

# Create executable
add_executable(api_gateway ${SRC_FILES})

# Link Aeron statically (preferred)
target_link_libraries(api_gateway
    Drogon::Drogon
    ${AERON_BUILD_DIR}/lib/libaeron_client.a  # ✅ Use correct path to the .a file
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libaeronWrapper.a
    pthread
    nlohmann_json::nlohmann_json
)

# Optional: set runtime path for shared libs if you ever use .so
# set_target_properties(api_gateway PROPERTIES
#     BUILD_RPATH ${AERON_BUILD_DIR}/lib
# )
