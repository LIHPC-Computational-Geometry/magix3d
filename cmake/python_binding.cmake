# Version 0.2 (support Python 2/Python 3)

include (GNUInstallDirs)
find_package (SWIG 3 REQUIRED)
# Par défaut on utilise Python 2

#find_package (Python REQUIRED COMPONENTS Interpreter Development)	# Rem : Python3 a la priorité => inutilisé car empêche l'accès à Python2
if (USE_PYTHON_3)
	find_package (Python3 REQUIRED COMPONENTS Interpreter Development)
	set (PYTHON_BINDING_DIR ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/)
	set (Python_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
	set (Python_EXECUTABLE ${Python3_EXECUTABLE})
	set (Python_VERSION ${Python3_VERSION})
	set (Python_LIBRARIES ${Python3_LIBRARIES})
	set (Python_LIBRARY_DIRS ${Python3_LIBRARY_DIRS})
else (USE_PYTHON_3)
	find_package (Python2 REQUIRED COMPONENTS Interpreter Development)
	set (PYTHON_BINDING_DIR ${CMAKE_INSTALL_LIBDIR}/python${Python2_VERSION_MAJOR}.${Python2_VERSION_MINOR}/site-packages/)
	set (Python_INCLUDE_DIRS ${Python2_INCLUDE_DIRS})
	set (Python_EXECUTABLE ${Python2_EXECUTABLE})
	set (Python_VERSION ${Python2_VERSION})
	set (Python_LIBRARIES ${Python2_LIBRARIES})
	set (Python_LIBRARY_DIRS ${Python2_LIBRARY_DIRS})
endif (USE_PYTHON_3)
include (${SWIG_USE_FILE})


# Répertoire d'installation des modules (pour le RPATH) :
set (CMAKE_PYTHON_RPATH_DIR ${CMAKE_INSTALL_PREFIX}/${PYTHON_BINDING_DIR})


