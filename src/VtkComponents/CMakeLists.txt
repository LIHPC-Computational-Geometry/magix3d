set (CURRENT_PACKAGE_NAME "VtkComponents")
set (CURRENT_PACKAGE_MACRO_NAME "VTK_COMPONENTS")

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (VtkContrib REQUIRED)

file (GLOB HEADERS protected/VtkComponents/vtkECMFactory.h protected/VtkComponents/vtkECMXOpenGLRenderWindow.h protected/VtkComponents/vtkECMXOpenGLRenderWindow.h
                   protected/VtkComponents/vtkLockableRenderWindow.h protected/VtkComponents/vtkUnstructuredGridRefinementFilter.h)
file (GLOB CPP_SOURCES vtkECMFactory.cpp vtkECMXOpenGLRenderWindow.cpp vtkECMXOpenGLRenderWindow.cpp vtkLockableRenderWindow.cpp vtkUnstructuredGridRefinementFilter.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas, par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (VtkComponents ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS VtkComponents)
set_property (TARGET VtkComponents PROPERTY VERSION ${MAGIX3D_VERSION})
set_property (TARGET VtkComponents PROPERTY SOVERSION ${MAGIX3D_MAJOR_VERSION})
# VTKGL : pour tout VTK >= 7 avec backend OpenGL 1, que ce soit OpenGL ou Mesa
set (VTK_COMPONENTS_PUBLIC_FLAGS "${MANDATORY_CXX_OPTIONS} -DVTKGL")
set (VTK_COMPONENTS_PRIVATE_FLAGS -DMAGIX3D_VERSION="${MAGIX3D_VERSION}" "${MANDATORY_VTK_OPTIONS}")

target_include_directories (VtkComponents PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (VtkComponents PUBLIC ${VTK_COMPONENTS_PUBLIC_FLAGS})
target_compile_definitions (VtkComponents PRIVATE ${VTK_COMPONENTS_PRIVATE_FLAGS})
target_link_libraries (VtkComponents PUBLIC VtkContrib::VtkContrib)
if (VTK_8)
	target_link_libraries (VtkComponents PUBLIC vtkRenderingParallel vtkRenderingLOD)
endif (VTK_8)

# Etre capable une fois installée de retrouver vtk*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libVtkComponents.so.5.0.0" to "") ...
set_target_properties (VtkComponents PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/VtkComponentsDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/VtkComponentsDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/VtkComponentsDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/VtkComponentsDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nous recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite, notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des règles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install (DIRECTORY protected/VtkComponents
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} 
         FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )
         
# Dont VtkComponentsDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/VtkComponentsDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de VtkComponents :
set (INCLUDE_INSTALL_DIR include/)				# VtkComponents_INCLUDE_DIR avec VtkComponentsConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# VtkComponents_LIB_DIR avec VtkComponentsConfig.cmake.in
# Création du fichier VtkComponentsConfig.cmake qui définira VtkComponents_INCLUDE_DIR, VtkComponents_LIB_DIR et VtkComponents_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier VtkComponentsTargets.cmake dans prefix/lib/VtkComponents/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers VtkComponentsConfig.cmake et VtkComponentsConfigVersion.cmake dans prefix/lib/VtkComponents/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})
