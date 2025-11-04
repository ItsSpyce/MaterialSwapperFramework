cmake_minimum_required(VERSION 3.21)

project(MaterialSwapperFramework VERSION 0.1.7.0)

set(CMAKE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party)
set(LIBXML2_WITH_ICONV OFF CACHE BOOL "Build libxml2 with iconv support")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(DEFINED ENV{SKYRIM_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_FOLDER}/Data")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_FOLDER}/Data")
endif()

if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME}")
endif()

add_compile_definitions(SKYRIM)
add_compile_definitions(SKYRIM_AE)
set(CommonLibPath "${CMAKE_LIB_DIR}/commonlibsse-ng")
set(CommonLibName "CommonLibSSE")
set(GameVersion "Skyrim")

include(CommonLibSSE)
include(CPM)
include(nuklear)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")

set(ENABLE_SKYRIM_SE ON)
set(ENABLE_SKYRIM_AE ON)
set(ENABLE_SKYRIM_VR OFF)
set(BUILD_TESTS OFF)

if(MSVC)
    add_compile_definitions(
        _UNICODE
    )

    if(NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
        add_compile_options(
            /MP # Build with Multiple Processes
        )
    endif()
endif()

set(LcuiPath ${CMAKE_LIB_DIR}/lcui)

find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_path(SRELL_INCLUDE_DIRS "srell.hpp")
find_package(directxtex CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)
find_package(binary_io CONFIG REQUIRED)
find_package(detours)

add_subdirectory(${CommonLibPath} EXCLUDE_FROM_ALL)

CPMAddPackage("gh:stephenberry/glaze@5.7.1")
CPMAddPackage(
  NAME task-thread-pool
  URL https://github.com/alugowski/task-thread-pool/archive/refs/heads/main.tar.gz
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SRELL_INCLUDE_DIRS})
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_commonlibsse_plugin(${PROJECT_NAME}
  SOURCES ${HEADERS} ${SOURCES}
  MINIMUM_SKSE_VERSION 0.0.0.0
)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE
  spdlog::spdlog
  imgui::imgui
  glaze::glaze
  Microsoft::DirectXTex
  Microsoft::DirectXTK
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/lib/detours.lib>
)

# When your SKSE .dll is compiled, this will automatically copy the .dll into your mods folder.
# Only works if you configure DEPLOY_ROOT above (or set the SKYRIM_MODS_FOLDER environment variable)
if(DEFINED OUTPUT_FOLDER)
    # If you specify an <OUTPUT_FOLDER> (including via environment variables)
    # then we'll copy your mod files into Skyrim or a mod manager for you!

    # Copy the SKSE plugin .dll files into the SKSE/Plugins/ folder
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Material Swapper Framework")
    file(MAKE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    # TODO: make this actually work with the res folder

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        # Ensure SKSE/Plugins directory exists in archive output
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins"
        # Copy res folder into SKSE/Plugins/<ProjectName> in archive output
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/res" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins/${PROJECT_NAME}"
        # Copy interface folder into archive output
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/interface" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/interface"
        # Copy DLL and PDB into SKSE/Plugins in archive output
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
        # Copy the entire archive output directory into OUTPUT_FOLDER
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" "${OUTPUT_FOLDER}"
        # Zip the archive output directory
        COMMAND "${CMAKE_COMMAND}" -E tar "cf" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.zip" --format=zip "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
        VERBATIM
    )
endif()