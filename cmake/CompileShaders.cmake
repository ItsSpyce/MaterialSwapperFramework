# Compiles shader files in src/Shaders to header files in src/Shaders/Generated using DirectX Shader Compiler
function(compile_shaders)
  file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "src/Shaders/*.hlsl")
  set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/Generated")
  set(GENERATED_HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/Generated.h")
  file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
  MESSAGE(STATUS "Shader files: ${SHADER_FILES}")

  # Prepare list of includes for Generated.h
  set(INCLUDE_LINES "#pragma once\n\n")
  foreach(SHADER_FILE ${SHADER_FILES})
      get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
      set(OUTPUT_HEADER "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.h")
      add_custom_command(
          PRE_BUILD
          OUTPUT ${OUTPUT_HEADER}
          COMMAND fxc /nologo /T ps_5_0 /E main /O3 /Zpc /Ges /Fh ${OUTPUT_HEADER} /Vn ${SHADER_NAME} /Qstrip_reflect /Qstrip_debug /Qstrip_priv ${SHADER_FILE}
          DEPENDS ${SHADER_FILE}
          COMMENT "Compiling shader ${SHADER_NAME}.hlsl to ${OUTPUT_HEADER}"
          VERBATIM
      )
      list(APPEND GENERATED_HEADERS ${OUTPUT_HEADER})
      string(APPEND INCLUDE_LINES "#include \"Shaders/Generated/${SHADER_NAME}.h\"\n")
  endforeach()

  # Write the includes to Generated.h
  file(WRITE ${GENERATED_HEADER_LIST} "${INCLUDE_LINES}")

  add_custom_target(Shaders ALL DEPENDS ${GENERATED_HEADERS} ${GENERATED_HEADER_LIST})
  add_dependencies(${PROJECT_NAME} Shaders)
endfunction()