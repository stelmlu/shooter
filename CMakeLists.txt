cmake_minimum_required(VERSION 3.10)
project(Shooter)

# set(CMAKE_FIND_DEBUG_MODE 1)

set(CMAKE_CXX_STANDARD 17) # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure C++17 is strictly required

# Find the SDL2 library
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} "/opt/homebrew/include/SDL2")
link_directories("/opt/homebrew/lib")

set(SDL2_IMAGE_DIRS "/opt/homebrew/include/SDL2")
find_package(SDL2_image REQUIRED)

# Include the SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Define your project's executable
add_executable(${PROJECT_NAME} Main.cpp)

# Link against the SDL2 library
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image)
