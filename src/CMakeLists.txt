add_subdirectory(matrix)
add_subdirectory(io)

set(MATRIX_SOURCES
    ${MATRIX_SOURCES}
    PARENT_SCOPE)

set(IO_SOURCES
    ${IO_SOURCES}
    PARENT_SCOPE)

set(SIPL_SOURCES
    ${SIPL_SOURCES}
    ${MATRIX_SOURCES}
    ${IO_SOURCES}
    PARENT_SCOPE)
