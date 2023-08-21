set (CURRENT_PACKAGE_NAME "Magix3DBatch_exe")

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/python_binding.cmake)

set (TARGET_NAME Magix3DBatch)
set (MAGIX3D_BATCH_EXE ${TARGET_NAME}-${MAGIX3D_VERSION})
add_executable (${MAGIX3D_BATCH_EXE} Magix3DBatch.cpp)

target_link_libraries (${MAGIX3D_BATCH_EXE} PUBLIC QtVtkComponents)
target_link_libraries (${MAGIX3D_BATCH_EXE} PUBLIC z)	# Pour Qt sous Ubuntu installé via l'installeur Qt)

# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
set_target_properties (${MAGIX3D_BATCH_EXE} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})

# INSTALLATION :
include(CMakePackageConfigHelpers)
# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

install(TARGETS ${MAGIX3D_BATCH_EXE} EXPORT Magix3D DESTINATION ${CMAKE_INSTALL_BINDIR})

# Utilisation par d'autres modules de Magix3DBatch :
set (RUNTIME_INSTALL_DIR bin/)		# Magix3DBatch_RUNTIME_DIR avec Magix3DBatch_exeConfig.cmake.in
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


# Le script de lancement de Magix3DBatch :
set (MAGIX3D_BINDING_INSTALL_DIR ${PYTHON_BINDING_DIR})
find_package (VTK REQUIRED COMPONENTS vtksys )
set (VTK_LIBRARY_PATH "${VTK_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
configure_file(cmake/${TARGET_NAME}.in
        ${CMAKE_CURRENT_BINARY_DIR}/Magix3DBatch/${TARGET_NAME} @ONLY
    )
install (
   FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Magix3DBatch/${TARGET_NAME}
   DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
   )


