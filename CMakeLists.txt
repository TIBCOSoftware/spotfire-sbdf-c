CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
PROJECT(SBDF C)

SET(BUILD_SHARED_LIBS ON CACHE BOOL "Whether to build shared or static libraries.")

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

INCLUDE_DIRECTORIES(include)
ADD_LIBRARY(sbdf
    src/bswap.c
    src/bytearray.c
    src/columnmetadata.c
    src/columnslice.c
    src/errors.c
    src/fileheader.c
    src/internals.c
    src/metadata.c
    src/object.c
    src/sbdfstring.c
    src/tablemetadata.c
    src/tableslice.c
    src/valuearray.c
    src/valuetype.c
)
SET_PROPERTY(TARGET sbdf PROPERTY DEFINE_SYMBOL "SBDF_EXPORTS")
IF(NOT BUILD_SHARED_LIBS)
    SET_PROPERTY(TARGET sbdf APPEND PROPERTY COMPILE_DEFINITIONS "SBDF_STATIC")
    SET_PROPERTY(TARGET sbdf APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS "SBDF_STATIC")
ENDIF()

ENABLE_TESTING()
ADD_SUBDIRECTORY(tests)
