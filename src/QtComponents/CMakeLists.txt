set (CURRENT_PACKAGE_NAME "QtComponents")
set (CURRENT_PACKAGE_MACRO_NAME "QT_COMPONENTS")

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (PrefsQt REQUIRED)
find_package (QtQualif REQUIRED)
find_package (GQGMDS REQUIRED)
find_package (Lima REQUIRED)
if (NOT USE_PYTHON_3)
	find_package (QtPython REQUIRED)
	set (QT_PYTHON_TARGET "QtPython::QtPython")
else (NOT USE_PYTHON_3)
	find_package (QtPython3 REQUIRED)
	set (QT_PYTHON_TARGET "QtPython3::QtPython3")
endif (NOT USE_PYTHON_3)

file (GLOB HEADERS protected/QtComponents/*.h)
file (GLOB CPP_SOURCES *.cpp *.qrc)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (QtComponents ${CPP_SOURCES} ${HEADERS})
set_property (TARGET QtComponents PROPERTY AUTOMOC ON)
set_property (TARGET QtComponents PROPERTY AUTORCC ON)
set (ALL_TARGETS QtComponents)
set_property (TARGET QtComponents PROPERTY VERSION ${MAGIX3D_VERSION})
set_property (TARGET QtComponents PROPERTY SOVERSION ${MAGIX3D_MAJOR_VERSION})

# Ugly code : on détermine ici si on aura une implémentation des chambres d'expériences. Si ça n'est pas
# le cas on ne compile pas le code correspondant. A noter que ce package ExperimentalRoom n'est appelé 
# que dans QtVtkComponents.
find_package (ExperimentalRoom)
if (ExperimentalRoom_FOUND)
	set_property (TARGET QtComponents PROPERTY AUTOMOC_MOC_OPTIONS "-DUSE_EXPERIMENTAL_ROOM")
	target_compile_options (QtComponents PUBLIC -DUSE_EXPERIMENTAL_ROOM)
endif (ExperimentalRoom_FOUND)

target_include_directories (QtComponents PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_options (QtComponents PRIVATE -DMAGIX3D_VERSION="${MAGIX3D_VERSION}")
target_compile_options (QtComponents PRIVATE -DSWIG_VERSION="${SWIG_VERSION}")
target_compile_options (QtComponents PRIVATE -DREPRESENTATION_TYPE_BY_POPUP)
target_compile_options (QtComponents PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (QtComponents PUBLIC Core pyMagix3D PrefsQt::PrefsQt ${QT_PYTHON_TARGET} QtQualif::QtQualif GQGMDS::GQGMDS)
# Etre capable une fois installée de retrouver TkUtil, Qt5*, ... :
set_target_properties (QtComponents PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/QtComponentsDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/QtComponentsDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/QtComponentsDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/QtComponentsDistribution.h)

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
install (DIRECTORY protected/QtComponents
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} 
         FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )
# Dont QtComponentsDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QtComponentsDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de QtComponents :
set (INCLUDE_INSTALL_DIR include/)		# QtComponents_INCLUDE_DIR avec QtComponentsConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# QtComponents_LIB_DIR avec QtComponentsConfig.cmake.in
# Création du fichier QtComponentsConfig.cmake qui définira QtComponents_INCLUDE_DIR, QtComponents_LIB_DIR et QtComponents_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier QtComponentsTargets.cmake dans prefix/lib/QtComponents/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers QtComponentsConfig.cmake et QtComponentsConfigVersion.cmake dans prefix/lib/QtComponents/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})
