# Nuklear is a git submodule in lib/nuklear written in C
set(NUKLEAR_PATH "${CMAKE_LIB_DIR}/nuklear")
# Single header file
set(SOURCES "${NUKLEAR_PATH}/nuklear.h")
include_directories(${NUKLEAR_PATH})
