set (CURRENT_PACKAGE_NAME "Core")
set (CURRENT_PACKAGE_MACRO_NAME "CORE")
include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)

find_package (GUIToolkitsVariables REQUIRED)
include (${GUIToolkitsVariables_CMAKE_DIR}/python_binding.cmake)

file (GLOB HEADERS protected/*/*.h)
file (GLOB CPP_SOURCES Internal/*.cpp Geom/*.cpp Topo/*.cpp Mesh/*.cpp Structured/*.cpp Group/*.cpp SysCoord/*.cpp Smoothing/*.cpp )
list(FILTER CPP_SOURCES EXCLUDE REGEX ".*GMDSGeom.*Adapter\\.cpp$")

# Répertoire d'en-têtes installés. Certains sont optionnels et ajoutés plus loin.
set (HEADERS_DIRECTORIES protected/Geom protected/Group protected/Internal protected/Mesh protected/Smoothing protected/Structured protected/SysCoord protected/Topo)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (Core ${CPP_SOURCES} ${HEADERS})

set (CORE_OPTIONAL_LIBRARIES_LIST)

find_package (PrefsCore REQUIRED)
find_package (PythonUtil REQUIRED)
find_package (GQGMDS REQUIRED)
#find_package (MachineTypes REQUIRED)
find_package (gmds REQUIRED)
target_compile_options (Core PUBLIC -DGMDS_VERSION="${gmds_VERSION}")
find_package (Lima REQUIRED)
message (STATUS "------------> LIMA VERSION ${Lima_VERSION}")
target_compile_options (Core PUBLIC -DLIMA_VERSION="${Lima_VERSION}")
if (MDLPARSER)
	find_package (MdlParser REQUIRED)
	message (STATUS "------------> MDLPARSER VERSION ${MdlParser_VERSION}")
	target_compile_options (Core PUBLIC -DMDL_VERSION="${MdlParser_VERSION}")
	list (APPEND CORE_OPTIONAL_LIBRARIES_LIST MdlParser::mdl)
endif (MDLPARSER)
if (SMOOTH3D)
	find_package (smooth3D REQUIRED)
	message (STATUS "------------> SMOOTH3D VERSION ${smooth3D_VERSION}")
	target_compile_options (Core PUBLIC -DSMOOTH3D_VERSION="${smooth3D_VERSION}")
	list (APPEND CORE_OPTIONAL_LIBRARIES_LIST smooth3D)
endif (SMOOTH3D)
if (SEPA3D)
	find_package (sepa3dcore REQUIRED)
	message (STATUS "------------> SEPA3D VERSION ${sepa3dcore_VERSION}")
	target_compile_options (Core PUBLIC -DSEPA3D_VERSION="${sepa3dcore_VERSION}")
	list (APPEND CORE_OPTIONAL_LIBRARIES_LIST sepa3dcore)
endif (SEPA3D)
if (DKOC)
	find_package (dkoc REQUIRED)
	message (STATUS "------------> DKOC VERSION ${dkoc_VERSION}")
	target_compile_definitions (Core PUBLIC -DDKOC_VERSION="${DKOC_VERSION}")
	target_include_directories (Core PUBLIC ${DKOC_INCLUDE_DIRS})
	list (APPEND CORE_OPTIONAL_LIBRARIES_LIST ${DKOC_LIBRARIES})
        #list (APPEND CORE_OPTIONAL_LIBRARIES_LIST ${DKOC_LIBRARY_IMPL}) # no need after dkoc-22.4
endif (DKOC)
#if (USE_TRITON)
#	find_package (Triton2 REQUIRED)
#	message (STATUS "------------> TRITON2 VERSION ${Triton2_VERSION}")
#	target_compile_definitions (Core PUBLIC -DTRITON_VERSION="${Triton2_VERSION}")
#	target_include_directories (Core PUBLIC ${TRITON2_INCLUDE_DIR})
#	list (APPEND CORE_OPTIONAL_LIBRARIES_LIST Triton2)
#endif (USE_TRITON)

find_package(PkgConfig REQUIRED)
include (${GUIToolkitsVariables_CMAKE_DIR}/python_binding.cmake)	# Access to Python.h
include (${CMAKE_SOURCE_DIR}/cmake/FindCgns.cmake)					# Also requires HDF5, brought by Lima
include (${CMAKE_SOURCE_DIR}/cmake/FindMesquite.cmake)
if (NOT TARGET Mesquite::Mesquite)
	message (FATAL_ERROR "------------> MESQUITE NOT FOUND, FATAL ERROR")
endif (NOT TARGET Mesquite::Mesquite)

pkg_check_modules (GTS REQUIRED IMPORTED_TARGET gts)  # GTS (GNU Triangulated Surface Library)
pkg_check_modules (GLIB2 REQUIRED IMPORTED_TARGET glib-2.0) # glib-2.0 (pour GTS)
pkg_check_modules (PCRE REQUIRED IMPORTED_TARGET libpcre)  # PCRE (Perl compatible regular expressions pour glib-2.0)

find_package (OpenCASCADE 7.1.0 REQUIRED)

set (ALL_TARGETS Core)
set_property (TARGET Core PROPERTY VERSION ${CORE_VERSION})
set_property (TARGET Core PROPERTY SOVERSION ${CORE_MAJOR_VERSION})
set (MAGIX3D_CORE_PUBLIC_FLAGS "${MANDATORY_CXX_OPTIONS} -DPYTHON_INTERPRETER=\"${Python_EXECUTABLE}\"")
set (MAGIX3D_CORE_PRIVATE_FLAGS -DMAGIX3D_CORE_VERSION="${MAGIX3D_VERSION}")

target_include_directories (Core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (Core PUBLIC ${MAGIX3D_CORE_PUBLIC_FLAGS})
target_compile_definitions (Core PRIVATE ${MAGIX3D_CORE_PRIVATE_FLAGS})
target_include_directories (Core PUBLIC ${Python_INCLUDE_DIRS})
target_include_directories (Core PUBLIC ${GMDS_INCLUDE_DIR})
target_include_directories (Core PUBLIC ${OpenCASCADE_INCLUDE_DIR})
target_link_libraries (Core PUBLIC ${OpenCASCADE_FoundationClasses_LIBRARIES} ${OpenCASCADE_ModelingAlgorithms_LIBRARIES} ${OpenCASCADE_DataExchange_LIBRARIES})

target_link_libraries (Core PUBLIC GQGMDS::GQGMDS Lima::Lima Utils PrefsCore::PrefsCore PythonUtil::PythonUtil)
if (USE_TRITON)
	target_link_libraries (Core PUBLIC Triton2)
endif()
if (USE_SMOOTH3D)
	target_link_libraries (Core PUBLIC smooth3D)
endif()
message (STATUS "------------> USE_GMSH ${USE_GMSH}")
if (USE_GMSH)
	include (${CMAKE_SOURCE_DIR}/cmake/FindGmsh.cmake)
	if (NOT TARGET Gmsh)
		message (FATAL_ERROR "------------> GMSH NOT FOUND, FATAL ERROR")
	endif (NOT TARGET Gmsh)
	target_link_libraries(Core PUBLIC Gmsh)
	target_compile_definitions (Core PRIVATE "-DUSE_GMSH")
endif()
if (SEPA3D)
    target_include_directories (Core PUBLIC ${SEPARATRICE3D_INCLUDE_DIR})
endif (SEPA3D)
target_link_libraries (Core PUBLIC ${CORE_OPTIONAL_LIBRARIES_LIST} Mesquite::Mesquite cgns::cgns PkgConfig::GTS PkgConfig::GLIB2 PkgConfig::PCRE GMDSIg GMDSIo)
# Etre capable une fois installée de retrouver vtk*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libCore.so.5.0.0" to "") ...
set_target_properties (Core PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/CoreDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/CoreDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/CoreDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/CoreDistribution.h)

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
install (DIRECTORY ${HEADERS_DIRECTORIES}
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} 
         FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )
# Dont CoreDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CoreDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de Core :
set (INCLUDE_INSTALL_DIR include/)		# Core_INCLUDE_DIR avec CoreConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# Core_LIB_DIR avec CoreConfig.cmake.in
# Création du fichier CoreConfig.cmake qui définira Core_INCLUDE_DIR, Core_LIB_DIR et Core_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier CoreTargets.cmake dans prefix/lib/Core/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers CoreConfig.cmake et CoreConfigVersion.cmake dans prefix/lib/Core/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation})


# Add debugging option
option(USE_DEBUG "Enter debug mode" OFF)
if (USE_DEBUG)
	add_compile_definitions(_DEBUG2)
endif()
