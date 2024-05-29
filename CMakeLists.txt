cmake_minimum_required (VERSION 3.15)

project (Magix3D LANGUAGES CXX C)

include (GNUInstallDirs)
include (cmake/version.cmake)
include (cmake/common.cmake)


set (Magix3D_VERSION ${MAGIX3D_VERSION})

set (CMAKE_VERBOSE_MAKEFILE ON)
option (BUILD_SHARED_LIBS "Creation de bibliotheques dynamiques (defaut : ON)" ON)

#======================================================================================================================
# Options that are proposed to build the project using Cmake Build system
#======================================================================================================================
option (BUILD_SHARED_LIBS "create dynamic libraries (default : ON)" ON)
option (BUILD_MAGIX3D "Build the whole MGX Application, including Gui and python binding (default : ON)" ON)
option (USE_GMSH "Build with gmsh (default : ON)" ON)
option (USE_SMOOTH3D "Build the SMOOTH3D component (default : OFF)" OFF)
option (USE_TRITON "Build the TRITON component (default : ON)" ON)
option (WITH_DOC "Build the documentation (default : ON)" ON)
option (WITH_TEST "Enable testing (default:OFF)" OFF)
option (WITH_COV "Enable code coverage of the testing target (default:OFF)" OFF)
#Next options are only available on-site. Default value is OFF.
option (USE_DKOC "On-site. Build the DKOC component to read Catia files (on-site, default : OFF)" OFF)
option (USE_MDLPARSER "Build the mdl parser to read the 2D mdl format (on-site, default : OFF)" OFF)
option (USE_MESHGEMS "Build the MeshGems component to access tri/tet meshing functionalities (on-site, default : OFF)" OFF)
option (USE_SEPA3D "Build the SEPA3D component (on-site, default : OFF)" OFF)

#======================================================================================================================
# Coverage require to run testing codes
if(WITH_COV)
  set(WITH_TEST ON)
endif(WITH_COV)
if(WITH_TEST)
  enable_testing()
endif (WITH_TEST)
if(WITH_COV)
  include(cmake/CodeCoverage.cmake)
  set(CMAKE_CXX_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
  set(CMAKE_C_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
  set(COVERAGE_LCOV_EXCLUDES
          '/usr/*'
          '${CMAKE_SOURCE_DIR}/*/tst/*'
          '*/cmake-build-debug/*'
          '*/spack/*'
          '*/GMSH/*'
  )
  SETUP_TARGET_FOR_COVERAGE_LCOV(
          NAME code_cover
          EXECUTABLE ctest -R test_link
  )
endif(WITH_COV)
#======================================================================================================================
#tell cmake to generate the configure file that will be used to activate the previous option C++ balise in the code
configure_file(
                ${PROJECT_SOURCE_DIR}/cmake/mgx_config.h.in
                ${PROJECT_BINARY_DIR}/mgx_config.h
)
include_directories(${PROJECT_BINARY_DIR})
#======================================================================================================================
add_subdirectory (src)
if(WITH_TEST)
  add_subdirectory(test_link)
endif (WITH_TEST)
if(WITH_DOC)
  message (STATUS "===============> MGX Documentation")
  add_subdirectory (Docs)
endif()

if(USE_TRITON)
  message (STATUS "===============> TRITON TRITON")
endif(USE_TRITON)
