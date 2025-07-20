set(EXECUTABLE_NAME nixln-edit)

add_executable(${EXECUTABLE_NAME} main.c utils.c args.c sig_handler.c)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

include(CheckPIESupported)
check_pie_supported()
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY POSITION_INDEPENDENT_CODE TRUE)
