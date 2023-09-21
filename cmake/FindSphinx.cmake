# CMake find_package() Module for Sphinx documentation generator
# http://sphinx-doc.org/
#
# Example usage:
#
# find_package(Sphinx)
#
# If successful the following variables will be defined
# SPHINX_FOUND
# SPHINX_EXECUTABLE

find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build sphinx-build2
             DOC "Path to sphinx-build executable")

# Handle REQUIRED and QUIET arguments
# this will also set SPHINX_FOUND to true if SPHINX_EXECUTABLE exists
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx
                                  "Failed to locate sphinx-build executable"
                                  SPHINX_EXECUTABLE)

# Provide options for controlling different types of output
option(SPHINX_OUTPUT_HTML "Output standalone HTML files" ON)
option(SPHINX_OUTPUT_MAN "Output man pages" ON)

option(SPHINX_WARNINGS_AS_ERRORS "When building documentation treat warnings as errors" OFF)

find_package (Python COMPONENTS Interpreter)

if (Python_Interpreter_FOUND)
    # Check for Sphinx theme dependency for documentation
    foreach (component IN LISTS Sphinx_FIND_COMPONENTS)
        execute_process (
            COMMAND ${Python_EXECUTABLE} -c "import ${component}"
            ERROR_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE _Sphinx_BUILD_ERROR
            OUTPUT_QUIET
            RESULT_VARIABLE _Sphinx_BUILD_RESULT
        )
        if (_Sphinx_BUILD_RESULT EQUAL 0)
            message (STATUS "Sphinx component '${component}' available")
            set (Sphinx_${component}_FOUND TRUE)
        elseif (_Sphinx_BUILD_RESULT EQUAL 1)
            message (FATAL_ERROR "Sphinx component '${component}' is not available")
            set (Sphinx_${component}_FOUND FALSE)
        endif (_Sphinx_BUILD_RESULT EQUAL 0)
        unset (_Sphinx_BUILD_ERROR)
        unset (_Sphinx_BUILD_RESULT)
    endforeach (component)
endif (Python_Interpreter_FOUND)
