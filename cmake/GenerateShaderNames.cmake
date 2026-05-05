file(MAKE_DIRECTORY "${GENERATED_INCLUDE_DIR}")
file(WRITE "${GENERATED_SHADER_NAMES_HEADER}" "#pragma once\n\n#include <string_view>\n\nnamespace Graphics::ShaderNames {\n")

foreach(SHADER_FILE IN LISTS SHADER_FILES)
  get_filename_component(SHADER_NAME "${SHADER_FILE}" NAME_WE)
  string(MAKE_C_IDENTIFIER "${SHADER_NAME}" SHADER_IDENTIFIER)
  file(APPEND "${GENERATED_SHADER_NAMES_HEADER}" "inline constexpr std::string_view ${SHADER_IDENTIFIER} = \"${SHADER_NAME}\";\n")
endforeach()

file(APPEND "${GENERATED_SHADER_NAMES_HEADER}" "}\n")
