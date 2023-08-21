set (CURRENT_PACKAGE_NAME "Utils")
set (CURRENT_PACKAGE_MACRO_NAME "UTILS")
include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (TkUtil 5 REQUIRED)

file (GLOB HEADERS protected/Utils/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (Utils ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS Utils)
set_property (TARGET Utils PROPERTY VERSION ${MAGIX3D_VERSION})
set_property (TARGET Utils PROPERTY SOVERSION ${MAGIX3D_MAJOR_VERSION})
set (MAGIX3D_UTILS_PUBLIC_FLAGS "${MANDATORY_CXX_OPTIONS}")
set (MAGIX3D_UTILS_PRIVATE_FLAGS -DMAGIX3D_UTILS_VERSION="${MAGIX3D_VERSION}")

target_include_directories (Utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (Utils PUBLIC ${MAGIX3D_UTILS_PUBLIC_FLAGS})
target_compile_definitions (Utils PRIVATE ${MAGIX3D_UTILS_PRIVATE_FLAGS})
target_link_libraries (Utils PUBLIC TkUtil::TkUtil)

# Etre capable une fois installée de retrouver vtk*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libUtils.so.5.0.0" to "") ...
set_target_properties (Utils PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/UtilsDistribution.h.in ${CMAKE_CURRENT_BINARY_DIR}/UtilsDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/UtilsDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/UtilsDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nous recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des règles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install (DIRECTORY protected/Utils
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} 
         FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )
# Dont UtilsDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/UtilsDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de Utils :
set (INCLUDE_INSTALL_DIR include/)		# Utils_INCLUDE_DIR avec UtilsConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# Utils_LIB_DIR avec UtilsConfig.cmake.in
# Création du fichier UtilsConfig.cmake qui définira Utils_INCLUDE_DIR, Utils_LIB_DIR et Utils_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier UtilsTargets.cmake dans prefix/lib/Utils/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers UtilsConfig.cmake et UtilsConfigVersion.cmake dans prefix/lib/Utils/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})


