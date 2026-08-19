cmake_minimum_required(VERSION 3.21)

project(MaterialSwapperFramework VERSION 0.4.0)

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
add_compile_definitions(UI_USE_IMGUI) # for ImGui
#add_compile_definitions(UI_USE_PRISMA) # for Prisma
set(CommonLibPath "${CMAKE_LIB_DIR}/commonlibsse-ng")
set(CommonLibName "CommonLibSSE")
set(GameVersion "Skyrim")

include(CommonLibSSE)
include(CPM)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")
SET(GLZ_USE_AVX2 ON)

option(MSF_TESTS "Enable MaterialSwapperFramework tests" OFF)
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

find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_path(SRELL_INCLUDE_DIRS "srell.hpp")
find_package(directxtex CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)
find_package(binary_io CONFIG REQUIRED)
find_package(detours)
find_package(sigc++-3 CONFIG REQUIRED)
find_package(matchit CONFIG REQUIRED)

add_subdirectory(${CommonLibPath} EXCLUDE_FROM_ALL)

CPMAddPackage("gh:stephenberry/glaze@8.0.0")
CPMAddPackage(
  NAME task-thread-pool
  URL https://github.com/alugowski/task-thread-pool/archive/refs/heads/main.tar.gz
)
CPMAddPackage("gh:neargye/magic_enum@0.9.8")
CPMAddPackage("gh:ktprime/emhash@1.2.0")

file(GLOB SHADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.hlsl")
set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_SHADER_NAMES_HEADER "${GENERATED_INCLUDE_DIR}/GeneratedShaderNames.h")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateShaderNames.cmake")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${GENERATED_INCLUDE_DIR}")
include_directories("${SRELL_INCLUDE_DIRS}")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
# third-party
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/third-party")
file(GLOB_RECURSE THIRD_PARTY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/*.cpp")

set(DEPENDENCIES
  spdlog::spdlog
  imgui::imgui
  glaze::glaze
  Microsoft::DirectXTex
  Microsoft::DirectXTK
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/lib/detours.lib>
  sigc-3.0
  magic_enum
  matchit::matchit
  emhash::emhash
)

add_commonlibsse_plugin(${PROJECT_NAME}
  SOURCES ${HEADERS} ${SOURCES} ${THIRD_PARTY_SOURCES}
  MINIMUM_SKSE_VERSION 0.0.0.0
)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDENCIES})

if(INCLUDE_TESTS)
  find_package(Catch2 3 CONFIG REQUIRED)
  include(CTest)
  include(Catch)

  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "test/*.cpp")

  add_executable(tests ${TEST_SOURCES})
  target_compile_definitions(tests PRIVATE INCLUDE_TESTS)
  target_precompile_headers(tests PRIVATE src/PCH.h)
  target_compile_features(tests PRIVATE cxx_std_23)
  target_link_libraries(tests PRIVATE
    CommonLibSSE::CommonLibSSE
    ${DEPENDENCIES}
    Catch2::Catch2WithMain
  )
  catch_discover_tests(tests)
endif()

if(DEFINED OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Material Swapper Framework")
    set(SHADER_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins/${PROJECT_NAME}/shaders")
    set(COMPILED_SHADER_OUTPUT_DIRECTORY "${SHADER_OUTPUT_DIRECTORY}/compiled")
    file(MAKE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    # TODO: make this actually work with the res folder

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        # Ensure SKSE/Plugins directory exists in archive output
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins"
        # Copy res folder into SKSE/Plugins/<ProjectName> in archive output
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/res" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins"
        # Copy interface folder into archive output
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/interface" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/interface"
        # Copy DLL and PDB into SKSE/Plugins in archive output
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/SKSE/Plugins/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
        # Copy raw HLSL shaders into the plugin's packaged shaders directory
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${SHADER_OUTPUT_DIRECTORY}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${COMPILED_SHADER_OUTPUT_DIRECTORY}"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${SHADER_OUTPUT_DIRECTORY}"
        # Copy the entire archive output directory into OUTPUT_FOLDER
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" "${OUTPUT_FOLDER}"
        # Zip the archive output directory
        COMMAND "${CMAKE_COMMAND}" -E tar "cf" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.zip" --format=zip "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
        VERBATIM
    )
endif()
