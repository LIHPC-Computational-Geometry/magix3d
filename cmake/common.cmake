#
# common.cmake : instructions de compilation partagées par tous les CMakeLists.txt.
# include (version.cmake) ici ne fonctionnenent pas. Why ??? Ce fichier est déjà un
#fichier déjà inclus ???
# => include version.cmake avant celui-ci.
#

include (${CMAKE_SOURCE_DIR}/cmake/organization.cmake)


# Compilation :
add_definitions (-DMAGIX3D_VERSION="${MAGIX3D_VERSION}")

# Options liées au multithreading :
set (MANDATORY_MULTITHREADING_OPTIONS -DMULTITHREADED_APPLICATION -D_GLIBCXX_USE_SCHED_YIELD)

# Options Open Cascade :
set (MANDATORY_OCC_OPTIONS -DOCC_EXTENDED -DNG_OCCGEOMETRY -DUSE_STL_STREAM -DHAVE_IOSTREAM -DHAVE_FSTREAM -DHAVE_IOMANIP -DHAVE_LIMITS_H -D_OCC64)

set (MAGIX3D_OPTIONS)
if (MDLPARSER)
	message (STATUS "------------> MDLPARSER : ON")
	list (APPEND MAGIX3D_OPTIONS -DUSE_MDLPARSER)
else  (MDLPARSER)
	message (STATUS "------------> MDLPARSER : OFF")
endif (MDLPARSER)
if (DKOC)
	message (STATUS "------------> DKOC : ON")
	list (APPEND MAGIX3D_OPTIONS -DUSE_DKOC)
else  (DKOC)
	message (STATUS "------------> DKOC : OFF")
endif (DKOC)
if (MESHGEMS)
	message (STATUS "------------> MESHGEMS : ON")
	list (APPEND MAGIX3D_OPTIONS -DUSE_MESHGEMS)
else  (MESHGEMS)
	message (STATUS "------------> MESHGEMS : OFF")
endif (MESHGEMS)
if (SEPA3D)
	message (STATUS "------------> SEPA3D : ON")
	list (APPEND MAGIX3D_OPTIONS -DUSE_SEPA3D)
else  (SEPA3D)
	message (STATUS "------------> SEPA3D : OFF")
endif (SEPA3D)
if (SMOOTH3D)
	message (STATUS "------------> SMOOTH3D : ON")
	list (APPEND MAGIX3D_OPTIONS -DUSE_SMOOTH3D)
else  (SMOOTH3D)
	message (STATUS "------------> SMOOTH3D : OFF")
endif (SMOOTH3D)
if (TRITON)
	message (STATUS "------------> TRITON : ON")
	list (APPEND MAGIX3D_OPTIONS -DUSE_TRITON)
else  (TRITON)
	message (STATUS "------------> TRITON : OFF")
endif (TRITON)

set (MANDATORY_CXX_OPTIONS ${MANDATORY_VTK_OPTIONS} ${MANDATORY_MULTITHREADING_OPTIONS} ${MAGIX3D_OPTIONS})

# Edition des liens :
# A l'installation les RPATHS utilisés seront ceux spécifiés ci-dessous (liste
# de répertoires séparés par des ;) :
#set (CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${MT_INSTALL_SHLIB_DIR})
#set (CMAKE_BUILD_WITH_INSTALL_RPATH ON)
# CMAKE_SKIP_BUILD_RPATH : avoir le rpath dans l'arborescence de developpement
set (CMAKE_SKIP_BUILD_RPATH OFF)
set (CMAKE_SKIP_RPATH OFF)

set (CMAKE_SKIP_INSTALL_RPATH OFF)
#set (CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

# ATTENTION : enleve le build tree du rpath :
#set (CMAKE_BUILD_WITH_INSTALL_RPATH ON)
