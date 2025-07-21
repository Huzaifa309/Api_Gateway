cmake_minimum_required(VERSION 3.15)
project(api_gateway)

set(CMAKE_CXX_STANDARD 17)

# Drogon dependencies
find_package(Drogon REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED)

# Aeron paths - Updated to point to the actual Aeron installation
set(AERON_SOURCE_DIR /home/it-admin/aeron-sbe-setup/aeron/aeron-client/src/main/cpp)
set(AERON_BUILD_DIR /home/it-admin/aeron-sbe-setup/aeron/build)

# Includes
include_directories(
    src
    ${AERON_SOURCE_DIR}             # Aeron C++ headers
    ${DROGON_INCLUDE_DIRS}
    external/concurrentqueue
    external/sbe/my_app_messages
    aeron/aeron-client/src/main/cpp
)

# All source files
file(GLOB SRC_FILES src/*.cpp)

# Create executable
add_executable(api_gateway ${SRC_FILES})

# Link Aeron statically (preferred)
target_link_libraries(api_gateway
    Drogon::Drogon
    ${AERON_BUILD_DIR}/lib/libaeron_client.a  # ✅ Use correct path to the .a file
    pthread
    nlohmann_json::nlohmann_json
)

# Optional: set runtime path for shared libs if you ever use .so
# set_target_properties(api_gateway PROPERTIES
#     BUILD_RPATH ${AERON_BUILD_DIR}/lib
# )
