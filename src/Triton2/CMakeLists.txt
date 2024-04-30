set (CURRENT_PACKAGE_NAME "Triton2")
set (CURRENT_PACKAGE_MACRO_NAME "TRITON2")

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (gmds REQUIRED)

file (GLOB HEADERS protected/Triton2/TetgenInterface/TetgenFacade.h protected/Triton2/TetgenInterface/tetgen.h)
file (GLOB CPP_SOURCES TetgenFacade.cpp tetgen/tetgen.cxx tetgen/predicates.cxx)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (Triton2 ${CPP_SOURCES} ${HEADERS})
set_property (TARGET Triton2 PROPERTY AUTOMOC ON)
set_property (TARGET Triton2 PROPERTY AUTORCC ON)
set (ALL_TARGETS Triton2)
set_property (TARGET Triton2 PROPERTY VERSION ${MAGIX3D_VERSION})
set_property (TARGET Triton2 PROPERTY SOVERSION ${MAGIX3D_MAJOR_VERSION})

target_include_directories (Triton2 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/Triton2Distribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/Triton2Distribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/Triton2Distribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/Triton2Distribution.h)

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
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# Dont Triton2Distribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Triton2Distribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de Triton2 :
set (INCLUDE_INSTALL_DIR include/)		# Triton2_INCLUDE_DIR avec Triton2Config.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# Triton2_LIB_DIR avec Triton2Config.cmake.in
# Création du fichier Triton2Config.cmake qui définira Triton2_INCLUDE_DIR, Triton2_LIB_DIR et Triton2_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier Triton2Targets.cmake dans prefix/lib/Triton2/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers Triton2Config.cmake et Triton2ConfigVersion.cmake dans prefix/lib/Triton2/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})
