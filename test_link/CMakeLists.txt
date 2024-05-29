#==============================================================================
project(mgx_python_testing)
cmake_minimum_required(VERSION 3.15)
#==============================================================================
enable_testing()
#==============================================================================
find_program(PYTEST_LOCATION pytest)
message("PYTEST_LOCATION= ${PYTEST_LOCATION}")
message("ENVIRONMENT PYTHONPATH ${PYTHONPATH}")
#message("ENVIRONMENT PYTHONPATH ${CMAKE_PREFIX_PATH}/lib/python3.10/site-packages:$PYTHONPATH")

add_test(NAME test_link
        COMMAND pytest -v -s ${CMAKE_CURRENT_SOURCE_DIR}
 )

if(WITH_COV)
    set_tests_properties(test_link
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/src/pyMagix3D;TUTO_SCRIPTS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../Docs/resources")
else(WITH_COV)
    set_tests_properties(test_link
	    PROPERTIES ENVIRONMENT "PYTHONPATH=$ENV{PYTHONPATH};TUTO_SCRIPTS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../Docs/resources")
endif (WITH_COV)
#==============================================================================
