set (CURRENT_PACKAGE_NAME "pyMagix3D")
set (CURRENT_PACKAGE_MACRO_NAME "PY_MAGIX3D")

enable_testing ( )

find_package (GUIToolkitsVariables)
include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/python_binding.cmake)

find_package (TkUtilScripting REQUIRED)

file (GLOB HEADERS protected/Python/*.h)
file (GLOB CPP_SOURCES pyMagix3D.i *.cpp)	# Pas swig_doc.i, inclus par pyMagix3D.i

# pyMagix3D est la cible "binding swig" => donnera _pyMagix3D.so et pyMagix3D.py. 
# Dans pyMagix3D.py on aura import.find_module('_pyMagix3D', [dirname(__file__)])
# => le nom du module (ici nom de la cible préfixé) est utilisé un peu partout (nom des fichiers, mais également
# par les sources du binding).
set (ALL_TARGETS pyMagix3D)
set_property (SOURCE pyMagix3D.i PROPERTY CPLUSPLUS ON)
# La cible, le module swig (_pyMagix3D.so) :
swig_add_library (pyMagix3D TYPE SHARED LANGUAGE PYTHON SOURCES ${CPP_SOURCES})
cmake_policy (SET CMP0086 NEW)
set_source_files_properties (pyMagix3D.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (pyMagix3D.i PROPERTIES SWIG_MODULE_NAME pyMagix3D)
# Ajouter une ligne précisant que pyMagix3D.py est encodé en UTF-8 (requis) :
add_custom_command (TARGET pyMagix3D POST_BUILD COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D.py ${CMAKE_CURRENT_BINARY_DIR}/binding.py COMMAND cat ${GUIToolkitsVariables_CMAKE_DIR}/utf8-header.py ${CMAKE_CURRENT_BINARY_DIR}/binding.py >> ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D.py)

# Objectif avoir les flags de compilation SWIG suivants :
# -globals pyMagix3D -shadow -DIN_STD="std::" -DSWIG_FORBIDS_FINALIZATION -DIN_UTIL="TkUtil::" -DIN_STD="std::"
# Ne marche pas sans le -c++ malgré les CPLUSPLUS ON avant le swig_add_library.
set (MAGIX3D_SWIG_COMPILE_OPTIONS "-c++;-module;pyMagix3D;-globals;pyMagix3D;-shadow;-DIN_STD=\"std::\";-DSWIG_FORBIDS_FINALIZATION;-DIN_UTIL=\"TkUtil::\";-DIN_STD=\"std::\"")
list (APPEND MAGIX3D_SWIG_COMPILE_OPTIONS ${MAGIX3D_OPTIONS})
# Avec cmake_policy (SET CMP0086 NEW) -module ${SWIG_MODULE_NAME} devrait être automatiquement passé, SWIG_MODULE_NAME étant défini comme propriété du fichier swig. Mais ...
#set (MT_SWIG_COMPILE_OPTIONS "-c++;-globals;pyMagix3D;-shadow;-DIN_STD=\"std::\";-DSWIG_FORBIDS_FINALIZATION;-DIN_UTIL=\"TkUtil::\";-DIN_STD=\"std::\"")
swig_link_libraries (pyMagix3D PUBLIC Core TkUtilScripting::TkUtilScripting)
foreach (lib ${ALL_TARGETS})
   target_include_directories (${lib} PRIVATE ${Python2_INCLUDE_DIRS})	# Ne devrait pas être nécesaire, non ? C'est pour le wrapper swig ...
   target_compile_options (${lib} PUBLIC ${MANDATORY_CXX_OPTIONS})
   target_compile_options (${lib} PRIVATE -DM3D_PYTHON_API_VERSION="${MAGIX3D_VERSION}")
   set_property (TARGET ${lib} PROPERTY VERSION ${MAGIX3D_VERSION})
   set_property (TARGET ${lib} PROPERTY SOVERSION ${MAGIX3D_MAJOR_VERSION})
   target_include_directories (${lib} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protected>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
   set_property (TARGET ${lib} PROPERTY SWIG_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src/Core/protected ${CMAKE_SOURCE_DIR}/src/Utils/protected)
   set_property (TARGET ${lib} PROPERTY SWIG_COMPILE_OPTIONS ${MAGIX3D_SWIG_COMPILE_OPTIONS})
# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
# Etre capable une fois installée de retrouver les libs utilisées, ... :
#message ("INSTALL RPATH=" "${CMAKE_PACKAGE_RPATH_DIR};${CMAKE_PYTHON_RPATH_DIR};$ORIGIN/../${PYTHON_BINDING_DIR}")
   set_target_properties (${lib} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH "${CMAKE_PACKAGE_RPATH_DIR};$ORIGIN/../${PYTHON_BINDING_DIR}")
   set_target_properties (${lib} PROPERTIES SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)
endforeach (lib)


# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/pyMagix3DDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3DDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/pyMagix3DDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3DDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nous recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# Création du fichier pyMagix3DConfig.cmake qui définira pyMagix3D_INCLUDE_DIR, pyMagix3D_LIB_DIR et pyMagix3D_RUNTIME_DIR (_pyMagix3D.so et .py) :
# On fait ici une installation "inhabituelle". S'agissant d'un binding python il est installé dans /install_prefix/lib/python/site-packages

# Installation de la bibliothèque :
install (TARGETS pyMagix3D EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} 
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install (DIRECTORY protected/Python
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} 
         FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
         )
# Dont pyMagix3DDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3DDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Installation du binding python :
# On ajoute en tête du fichier généré par swig l'encoding utilisé car sinon ça ne passe pas forcément lors de son exécution. Pour ce on le renomme en binding.py, puis on 
# fait un cat entete.py binding.py dont la sortie standard (re)devient pyMagix3D.py que l'on installera alors.
install (CODE "file (RENAME ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D.py ${CMAKE_CURRENT_BINARY_DIR}/binding.py)")
install (CODE "execute_process (COMMAND cat ${GUIToolkitsVariables_CMAKE_DIR}/utf8-header.py ${CMAKE_CURRENT_BINARY_DIR}/binding.py OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D.py)")
# Recopie du fichier binding python :
install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D.py DESTINATION ${PYTHON_BINDING_DIR})
# Le lien lib/_pyMagix3D.so.x.y.z vers lib/python/site-packages/_pyMagix3D.so.x.y.z n'est pas bon car ils ne sont plus dans le même répertoire ...
install (CODE "execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/_pyMagix3D.so.${MAGIX3D_VERSION} ${CMAKE_INSTALL_PREFIX}/${PYTHON_BINDING_DIR}/_pyMagix3D.so.${MAGIX3D_VERSION})")
install (CODE "execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/${PYTHON_BINDING_DIR}/_pyMagix3D.so.${MAGIX3D_VERSION} ${CMAKE_INSTALL_PREFIX}/${PYTHON_BINDING_DIR}/_pyMagix3D.so.${MAGIX3D_MAJOR_VERSION})")
install (CODE "execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_INSTALL_PREFIX}/${PYTHON_BINDING_DIR}/_pyMagix3D.so.${MAGIX3D_MAJOR_VERSION} ${CMAKE_INSTALL_PREFIX}/${PYTHON_BINDING_DIR}/_pyMagix3D.so)")

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file (${CMAKE_PACKAGE_VERSION_FILE} VERSION ${MAGIX3D_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de pyMagix3D :
set (INCLUDE_INSTALL_DIR include/)		# pyMagix3D_INCLUDE_DIR avec pyMagix3DConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# pyMagix3D_LIB_DIR avec pyMagix3DConfig.cmake.in
set (RUNTIME_INSTALL_DIR ${PYTHON_BINDING_DIR})	# pyMagix3D_RUNTIME_DIR avec pyMagix3DConfig.cmake.in
# Création du fichier pyMagix3DConfig.cmake qui définira pyMagix3D_INCLUDE_DIR, pyMagix3D_LIB_DIR et pyMagix3D_RUNTIME_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR RUNTIME_INSTALL_DIR)

# Copie du fichier pyMagix3DTargets.cmake dans prefix/lib/pyMagix3D/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation}
	)

# Copie des fichiers pyMagix3DConfig.cmake et pyMagix3DConfigVersion.cmake dans prefix/lib/pyMagix3D/cmake :
install (
   FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE}
   DESTINATION ${ConfigPackageLocation}
   )

#[===[
# La doc doxygen de l'API python :
find_package (Doxygen)
if (Doxygen_FOUND)
	set (DOXYGEN_PROJECT_NAME Magix3D)
	set (DOXYGEN_PROJECT_NUMBER ${MAGIX3D_VERSION})
	set (DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D)
	set (DOXYGEN_OUTPUT_LANGUAGE French)
	set (DOXYGEN_FULL_PATH_NAMES NO)
	set (DOXYGEN_MULTILINE_CPP_IS_BRIEF YES)
	set (DOXYGEN_TAB_SIZE 8)
	set (DOXYGEN_EXTRACT_LOCAL_CLASSES NO)
	set (DOXYGEN_SORT_BRIEF_DOCS YES)
	set (DOXYGEN_SHOW_USED_FILES NO)
	set (DOXYGEN_SHOW_FILES NO)
	set (DOXYGEN_SOURCES ${CMAKE_SOURCE_DIR}/src/Doc/User 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Internal/M3DCommandManager.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Internal/M3DCommandResultIfc.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Geom/GeomManagerIfc.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Geom/GeomInfo.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Internal/ContextIfc.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Point.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Spherical.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Vector.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Plane.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Rotation.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/RotX.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/RotY.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/RotZ.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Constants.h 
                         ${CMAKE_SOURCE_DIR}/src/Utils/protected/Utils/Unit.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/TopoInfo.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/TopoManagerIfc.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/CoEdgeMeshingProperty.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyUniform.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyGeometric.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertySpecificSize.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyInterpolate.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyGlobalInterpolate.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyHyperbolic.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyBigeometric.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyTabulated.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/EdgeMeshingPropertyBeta.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/CoFaceMeshingProperty.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyDirectional.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyOrthogonal.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyRotational.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyTransfinite.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyDelaunayNetgen.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyDelaunayGMSH.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/FaceMeshingPropertyQuadPairing.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingProperty.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyDirectional.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyOrthogonal.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyRotational.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyTransfinite.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyDelaunayNetgen.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyDelaunayTetgen.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Topo/BlockMeshingPropertyInsertion.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Mesh/MeshManagerIfc.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Smoothing/SurfacicSmoothing.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Smoothing/VolumicSmoothing.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Mesh/MeshModificationBySepa.h 
                         ${CMAKE_SOURCE_DIR}/src/Core/protected/Group/GroupManagerIfc.h
	)
	set (DOXYGEN_RECURSIVE YES)
	set (DOXYGEN_IMAGE_PATH ../Doc/Images)
	set (DOXYGEN_STRIP_CODE_COMMENTS NO)
	set (DOXYGEN_VERBATIM_HEADERS NO)
	set (DOXYGEN_ALPHABETICAL_INDEX NO)
	set (DOXYGEN_GENERATE_TREEVIEW YES)
	set (DOXYGEN_GENERATE_LATEX NO)
	set (DOXYGEN_PREDEFINED SWIG=1)
	set (DOXYGEN_HIDE_UNDOC_RELATIONS NO)
	set (DOXYGEN_HAVE_DOT YES)
	set (DOXYGEN_DOT_FONTNAME FreeSans)
	set (DOXYGEN_COLLABORATION_GRAPH NO)
	set (DOXYGEN_GENERATE_LEGEND NO)
	doxygen_add_docs (pydoc ${DOXYGEN_SOURCES} ALL)

	install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pyMagix3D
		DESTINATION ${CMAKE_INSTALL_DOCDIR}
         	FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ
         	DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETGID
		)

else (Doxygen_FOUND)
	message (WARNING "Documentation de l'API python non générée : doxygen non trouvé.")
#	message (FATAL_ERROR "Doxygen not found")
endif (Doxygen_FOUND)
]===]

# Le lancement du test de non régression via ctest : depuis le répertoire de build, faire ctest -V nr
#add_test (NAME nr COMMAND make default PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR} PYTHON_EXE=${Python2_EXECUTABLE} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../NonRegression)
