set (CURRENT_PACKAGE_NAME "Magix3D_exe")

find_package (GUIToolkitsVariables)
include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/python_binding.cmake)

find_package (Qt5Core)	# Beurk
set (TARGET_NAME Magix3D)
set (MAGIX3D_EXE ${TARGET_NAME}-${MAGIX3D_VERSION})
add_executable (${MAGIX3D_EXE} main.cpp)

target_link_libraries (${MAGIX3D_EXE} PUBLIC QtVtkComponents)
target_link_libraries (${MAGIX3D_EXE} PUBLIC pyMagix3D)	# !!! Déjà apporté par QtComponents ...
target_link_libraries (${MAGIX3D_EXE} PUBLIC z)	# Pour Qt sous Ubuntu installé via l'installeur Qt

# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
set_target_properties (${MAGIX3D_EXE} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1	INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})

# INSTALLATION :
include(CMakePackageConfigHelpers)
# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

install(TARGETS ${MAGIX3D_EXE} EXPORT Magix3D DESTINATION ${CMAKE_INSTALL_BINDIR})

# Utilisation par d'autres modules de Magix3D :
set (RUNTIME_INSTALL_DIR bin/)		# Magix3D_RUNTIME_DIR avec Magix3D_exeConfig.cmake.in
configure_package_config_file(cmake/${CURRENT_PACKAGE_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
        PATH_VARS RUNTIME_INSTALL_DIR
    )
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}ConfigVersion.cmake
        VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion
    )
install (
   FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}ConfigVersion.cmake
   DESTINATION ${ConfigPackageLocation}
   )

# Le script de lancement de Magix3D :
if (NOT USE_PYTHON_3)
	find_package (QtPython REQUIRED)
else (NOT USE_PYTHON_3)
	find_package (QtPython3 REQUIRED)
	set (QtPython_RUNTIME_DIR "${QtPython3_RUNTIME_DIR}")
endif (NOT USE_PYTHON_3)
#find_package (LimaScripting)	# Faut il ajouter le PATH à Lima si installé ailleurs ?
set (MAGIX3D_BINDING_INSTALL_DIR ${PYTHON_BINDING_DIR})
find_package (VTK REQUIRED COMPONENTS vtksys )
set (VTK_LIBRARY_PATH "${VTK_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
configure_file(cmake/${TARGET_NAME}.in ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${TARGET_NAME} @ONLY)
install (
   FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${TARGET_NAME}
   DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
   )

# La configuration :
install (DIRECTORY ${CMAKE_SOURCE_DIR}/src/etc
         DESTINATION ${CMAKE_INSTALL_PREFIX}
         FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )

configure_file(cmake/Magix3DPython.py.in ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/Magix3DPython.py @ONLY)
install (
    PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/Magix3DPython.py
    TYPE BIN
   )

