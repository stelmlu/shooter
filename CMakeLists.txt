cmake_minimum_required(VERSION 3.10)
project(Shooter)

# set(CMAKE_FIND_DEBUG_MODE 1)

set(CMAKE_CXX_STANDARD 17) # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure C++17 is strictly required

# Find the SDL2 library
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} "/opt/homebrew/include/SDL2" "/opt/homebrew/include/entt/")
link_directories("/opt/homebrew/lib")

set(SDL2_IMAGE_DIRS "/opt/homebrew/include/SDL2")
find_package(SDL2_image REQUIRED)

# Include the SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS} ${})

# Define your project's executable
add_executable(${PROJECT_NAME} Main.cpp GameEngine/Game.cpp Player.cpp)

# Link against the SDL2 library
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image)

# Gfx directory to copy
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/gfx")
set(DEST_DIR "${CMAKE_BINARY_DIR}")

if(UNIX)
    set(COPY_COMMAND rsync -ru --delete)
elseif(WIN32)
    set(COPY_COMMAND robocopy /E /xo /purge)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${COPY_COMMAND} ${SOURCE_DIR} ${DEST_DIR}
    COMMENT "Incrementally copying gfx directory to build directory"
)
