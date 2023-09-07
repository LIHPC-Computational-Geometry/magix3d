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
option (USE_TRITON "Build the TRITON component (default : ON)" ON)
#Next options are only available on-site. Default value is so OFF.
option (USE_DKOC "On-site. Build the DKOC component to read Catia files (on-site, default : OFF)" OFF)
option (USE_MDLPARSER "Build the mdl parser to read the 2D mdl format (on-site, default : OFF)" OFF)
option (USE_MESHGEMS "Build the MeshGems component to access tri/tet meshing functionalities (on-site, default : OFF)" OFF)
option (USE_SEPA3D "Build the SEPA3D component (on-site, default : OFF)" OFF)
option (USE_SMOOTH3D "Build the SMOOTH3D component (on-site, default : OFF)" OFF)
#======================================================================================================================
#tell cmake to generate the configure file that will be used to activate the previous option C++ balise in the code
configure_file(
                ${PROJECT_SOURCE_DIR}/cmake/mgx_config.h.in
                ${PROJECT_BINARY_DIR}/mgx_config.h
)
include_directories(${PROJECT_BINARY_DIR})
#======================================================================================================================
add_subdirectory (src)
message (STATUS "===============> MGX Documentation")
add_subdirectory (Docs)
