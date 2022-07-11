ENABLE_LANGUAGE(CXX)

INCLUDE(TestFramework.cmake)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_EXECUTABLE(runtest
	main.cpp
	test.cpp test.h test_c.h
	cppsupp.h
	object_test.cpp
	valuearray_test.cpp
	metadata_test.cpp
	tablemetadata_test.cpp
	string_test.cpp
	bytearray_test.cpp
	columnslice_test.cpp
	tableslice_test.cpp
	advanced_api_test.cpp
	api_scenario.c
	samplefile_test.cpp
	dumpcontents_test.cpp
)
SET_PROPERTY(SOURCE samplefile_test.cpp dumpcontents_test.cpp advanced_api_test.cpp PROPERTY COMPILE_DEFINITIONS "TESTSDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
TARGET_LINK_LIBRARIES(runtest sbdf)
ADD_TESTS_FROM(runtest)
