#
# organization.cmake : organisation des répertoires du projet
#


# L'installation :
# ================

# Le répertoire d'installation où sont mis les fichiers cmake à destination des utilisateurs (Module/cmake) :
set (CMAKE_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/${CURRENT_PACKAGE_NAME}/cmake)

# Répertoire d'installation des bibliothèques (pour le RPATH) :
set (CMAKE_PACKAGE_RPATH_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

# Les fichiers d'aide de Lem :
set (CMAKE_ETC_DIR_NAME "etc")
set (MAGIX3D_CFG_FILE "magix3d.xml")
if (PRODUCTION)	# Appeler cmake avec -DPRODUCTION=ON
	set (MAGIX3D_CFG_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_ETC_DIR_NAME}/${MAGIX3D_CFG_FILE}")
	message (STATUS "PRODUCTION=ON CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} => MAGIX3D_CFG_PATH=${MAGIX3D_CFG_PATH}")
else (PRODUCTION)
	set (MAGIX3D_CFG_PATH "${CMAKE_BINARY_DIR}/src/${CMAKE_ETC_DIR_NAME}/${MAGIX3D_CFG_FILE}")
	message (STATUS "PRODUCTION=OFF CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} => MAGIX3D_CFG_PATH=${MAGIX3D_CFG_PATH}")
endif (PRODUCTION)

# Module::
set (CMAKE_MODULE_NAMESPACE ${CURRENT_PACKAGE_NAME}::)

# ModuleConfig
set (CMAKE_PACKAGE_CONFIG_NAME ${CURRENT_PACKAGE_NAME}Config)

# ModuleTargets
set (CMAKE_PACKAGE_TARGETS_NAME ${CURRENT_PACKAGE_NAME}Targets)

# Le répertoire des fichiers de configuration (lib/Module/cmake) :
set (CMAKE_PACKAGE_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME})

# Le Fichier lib/Module/cmake/ModuleConfigVersion.cmake :
set (CMAKE_PACKAGE_CONFIG_FILE ${CMAKE_PACKAGE_CONFIG_DIR}/${CMAKE_PACKAGE_CONFIG_NAME}.cmake)

# Le Fichier lib/Module/cmake/ModuleConfigVersion.cmake :
set (CMAKE_PACKAGE_VERSION_FILE ${CMAKE_PACKAGE_CONFIG_DIR}/${CMAKE_PACKAGE_CONFIG_NAME}Version.cmake)

# Le Fichier lib/Module/cmake/ModuleTargets.cmake :
set (CMAKE_PACKAGE_TARGETS_BASENAME ${CMAKE_PACKAGE_TARGETS_NAME}.cmake)
set (CMAKE_PACKAGE_TARGETS_FILE ${CMAKE_PACKAGE_CONFIG_DIR}/${CMAKE_PACKAGE_TARGETS_BASENAME})
