set (CURRENT_PACKAGE_NAME "GMSH")
set (CURRENT_PACKAGE_MACRO_NAME "MGX3D_GMSH")
include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (OpenCASCADE 7.1.0 REQUIRED)

file (GLOB HEADERS protected/*.h)
file (GLOB CPP_SOURCES Common/*.cpp Geo/*.cpp Mesh/*.cpp Numeric/*.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (Gmsh ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS Gmsh)
set_property (TARGET Gmsh PROPERTY VERSION ${GMSH_VERSION})
set_property (TARGET Gmsh PROPERTY SOVERSION ${GMSH_MAJOR_VERSION})
set (MAGIX3D_GMSH_PUBLIC_FLAGS "${MANDATORY_CXX_OPTIONS}")
set (MAGIX3D_GMSH_PRIVATE_FLAGS -DMAGIX3D_GMSH_VERSION=\"${MAGIX3D_VERSION}\" ${MANDATORY_OCC_OPTIONS})

target_include_directories (Gmsh PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_include_directories (Gmsh PUBLIC ${OpenCASCADE_INCLUDE_DIR})
target_compile_definitions (Gmsh PUBLIC ${MAGIX3D_GMSH_PUBLIC_FLAGS})
target_compile_definitions (Gmsh PRIVATE ${MAGIX3D_GMSH_PRIVATE_FLAGS})
#target_link_libraries (Gmsh PUBLIC TKSTL TKV3d TKG2d TKHLR TKSTEP TKIGES TKXSBase TKShHealing TKService TKMath TKOffset TKBool TKPrim TKTopAlgo TKGeomAlgo TKBRep TKFeat TKGeomBase TKG3d TKG2d TKMath TKernel TKBO TKMesh TKXDEIGES TKXDESTEP TKCDF TKCAF TKXCAF)
target_link_libraries (Gmsh PUBLIC ${OpenCASCADE_FoundationClasses_LIBRARIES} ${OpenCASCADE_ModelingAlgorithms_LIBRARIES} ${OpenCASCADE_DataExchange_LIBRARIES})

# Quelques spécificité de compilation :
SET_SOURCE_FILES_PROPERTIES (Numeric/robustPredicates.cpp PROPERTIES COMPILE_FLAGS -O0)
SET_SOURCE_FILES_PROPERTIES (Mesh/BDS.cpp PROPERTIES COMPILE_FLAGS -O0)

# Etre capable une fois installée de retrouver vtk*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libGmsh.so.5.0.0" to "") ...
set_target_properties (Gmsh PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/GMSHDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/GMSHDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/GMSHDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/GMSHDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
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
# Dont GmshDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GMSHDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de Gmsh :
set (INCLUDE_INSTALL_DIR include/)		# Gmsh_INCLUDE_DIR avec GmshConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# Gmsh_LIB_DIR avec GmshConfig.cmake.in
# Création du fichier GmshConfig.cmake qui définira Gmsh_INCLUDE_DIR, Gmsh_LIB_DIR et Gmsh_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier GmshTargets.cmake dans prefix/lib/Gmsh/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers GmshConfig.cmake et GmshConfigVersion.cmake dans prefix/lib/Gmsh/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})


