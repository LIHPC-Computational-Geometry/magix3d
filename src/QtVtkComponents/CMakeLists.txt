set (CURRENT_PACKAGE_NAME "QtVtkComponents")
set (CURRENT_PACKAGE_MACRO_NAME "QT_VTK_COMPONENTS")

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (QtVtk 7)
find_package (ExperimentalRoom 7)

if (VTK_9)
	set (VTK_REQUIRED_PACKAGES RenderingLOD)
	find_package (VTK REQUIRED COMPONENTS ${VTK_REQUIRED_PACKAGES})
endif (VTK_9)
	
file (GLOB HEADERS protected/QtVtkComponents/*.h)
file (GLOB CPP_SOURCES *.cpp *.qrc)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (QtVtkComponents ${CPP_SOURCES} ${HEADERS})
set_property (TARGET QtVtkComponents PROPERTY AUTOMOC ON)
set (ALL_TARGETS QtVtkComponents)
set_property (TARGET QtVtkComponents PROPERTY VERSION ${MAGIX3D_VERSION})
set_property (TARGET QtVtkComponents PROPERTY SOVERSION ${MAGIX3D_MAJOR_VERSION})

target_include_directories (QtVtkComponents PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_options (QtVtkComponents PRIVATE -DMAGIX3D_VERSION="${MAGIX3D_VERSION}")
target_link_libraries (QtVtkComponents PUBLIC QtComponents VtkComponents QtVtk::QtVtk)
if (ExperimentalRoom_FOUND)
#	target_compile_options (QtVtkComponents PUBLIC -DUSE_EXPERIMENTAL_ROOM)	# Déjà fourni dans QtComponents
	target_link_libraries (QtVtkComponents PUBLIC ExperimentalRoom::ExperimentalRoom)
	find_package (ExperimentalRoomTExtension 7 REQUIRED)
	target_link_libraries (QtVtkComponents PUBLIC ExperimentalRoomTExtension::ExperimentalRoomTExtension)
endif (ExperimentalRoom_FOUND)

target_compile_options (QtVtkComponents PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
if (VTK_9)
	target_link_libraries (QtVtkComponents PRIVATE ${VTK_LIBRARIES})
endif (VTK_9)
# Etre capable une fois installée de retrouver TkUtil, Qt5*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libQtVtkComponents.so.5.0.0" to "") ...
set_target_properties (QtVtkComponents PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/QtVtkComponentsDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/QtVtkComponentsDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/QtVtkComponentsDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/QtVtkComponentsDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande trés vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des régles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install (DIRECTORY protected/QtVtkComponents
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} 
         FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )
# Dont QtVtkComponentsDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QtVtkComponentsDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de QtVtkComponents :
set (INCLUDE_INSTALL_DIR include/)		# QtVtkComponents_INCLUDE_DIR avec QtVtkComponentsConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/)	# QtVtkComponents_LIB_DIR avec QtVtkComponentsConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)		# QtVtkComponents_RUNTIME_DIR avec QtVtkComponentsConfig.cmake.in
# Création du fichier QtVtkComponentsConfig.cmake qui définira QtVtkComponents_INCLUDE_DIR, QtVtkComponents_LIB_DIR et QtVtkComponents_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier QtVtkComponentsTargets.cmake dans prefix/lib/QtVtkComponents/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers QtVtkComponentsConfig.cmake et QtVtkComponentsConfigVersion.cmake dans prefix/lib/QtVtkComponents/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})
