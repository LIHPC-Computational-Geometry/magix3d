# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Python REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED COMPONENTS sphinx_rtd_theme breathe sphinx_copybutton rst2pdf)

# Find all the public headers
get_target_property(MGX_PUBLIC_HEADER_DIR Core INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE MGX_PUBLIC_HEADERS ${MGX_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_DIRS
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Internal/Context.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Internal/M3DCommandManager.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Internal/M3DCommandResultIfc.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Point.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Spherical.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Cylindrical.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Vector.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Plane.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Rotation.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/RotX.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/RotY.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Point.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/RotZ.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Constants.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Landmark.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/Unit.h
  ${PROJECT_SOURCE_DIR}/src/Utils/protected/Utils/SwigCompletion.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Geom/GeomManager.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Geom/GeomInfo.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/TopoInfo.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/TopoManager.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyUniform.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyGeometric.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyBigeometric.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyHyperbolic.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertySpecificSize.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyInterpolate.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyGlobalInterpolate.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyTabulated.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyBeta.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyTransfinite.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyDelaunayGMSH.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyQuadPairing.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyTransfinite.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyDelaunayTetgen.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Mesh/MeshManager.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Mesh/MeshModificationBySepa.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Smoothing/SurfacicSmoothing.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Smoothing/VolumicSmoothing.h
  ${PROJECT_SOURCE_DIR}/src/Core/protected/Group/GroupsManager.h
)

string(REPLACE ";" " " DOXYGEN_DIRS_STR "${DOXYGEN_DIRS}")
set(DOXYGEN_INPUT_DIR ${DOXYGEN_DIRS_STR})
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

#Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

set(VARMMODULE_IN ${CMAKE_CURRENT_SOURCE_DIR}/variables.py.in)
set(VARMMODULE_OUT ${CMAKE_CURRENT_BINARY_DIR}/variables.py)

configure_file(${VARMMODULE_IN} ${VARMMODULE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
# fix bug https://github.com/doxygen/doxygen/issues/6293
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html)
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}/xml)

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${MGX_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs"
                   VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

# we call sphinx from camke command to set PYTHONPATH (to use built VARMMODULE_OUT file)
set(HTML_SPHINX_COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_CURRENT_BINARY_DIR}
      ${SPHINX_EXECUTABLE} -q -b html
      # Tell Breathe where to find the Doxygen output
    -Dbreathe_projects.MGX=${DOXYGEN_OUTPUT_DIR}/xml
      ${SPHINX_SOURCE} ${SPHINX_BUILD})

# we call sphinx from camke command to set PYTHONPATH (to use built VARMMODULE_OUT file)
set(PDF_SPHINX_COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_CURRENT_BINARY_DIR}
    ${SPHINX_EXECUTABLE} -b pdf
${SPHINX_SOURCE} ${SPHINX_BUILD}/pdf)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(
    OUTPUT ${SPHINX_INDEX_FILE}
    COMMAND ${HTML_SPHINX_COMMAND}
    COMMAND ${PDF_SPHINX_COMMAND}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
    # Other docs files you want to track should go here (or in some variable)
    ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/*.rst
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*
    ${CMAKE_CURRENT_SOURCE_DIR}/images/*
    ${CMAKE_CURRENT_SOURCE_DIR}/_ext/*.py
    ${CMAKE_CURRENT_SOURCE_DIR}/_static/*.css
    ${CMAKE_CURRENT_SOURCE_DIR}/*.yaml
    ${DOXYGEN_INDEX_FILE}
    MAIN_DEPENDENCY conf.py
    COMMENT "Generating html and pdf documentation with Sphinx")

# Doc target so we can run the job easily
add_custom_target(documentation ALL DEPENDS ${SPHINX_INDEX_FILE})

# Recopie de la doc générée dans le répertoire d'installation
include(GNUInstallDirs)
install (DIRECTORY ${SPHINX_BUILD}
		DESTINATION ${CMAKE_INSTALL_DOCDIR}
    FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ
		DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
   )

install (DIRECTORY ${DOXYGEN_OUTPUT_DIR}
		DESTINATION ${CMAKE_INSTALL_DOCDIR}
		FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ
		DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
    )
