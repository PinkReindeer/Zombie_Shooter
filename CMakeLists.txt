cmake_minimum_required(VERSION 3.28)
project(Zombie_Shooter)

if (NOT DEFINED PLATFORM)
    set(PLATFORM "Desktop" CACHE STRING "Target platform: Desktop or Web" FORCE)
endif()

message(STATUS "Building for PLATFORM=${PLATFORM}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set(VS_STARTUP_PROJECT ${PROJECT_SOURCE_DIR})
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

if ("${PLATFORM}" STREQUAL "Desktop")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -mwindows")
endif()

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

add_executable(${PROJECT_NAME}
    main.cpp
    Application.cpp
    Application.h
    Graphics/Renderer.h
    Graphics/Renderer.cpp
    GameData.h
    World.h
    World.cpp
    Game.h
    Game.cpp
    Random.h
  )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_precompile_headers(${PROJECT_NAME} PUBLIC gpch.h)