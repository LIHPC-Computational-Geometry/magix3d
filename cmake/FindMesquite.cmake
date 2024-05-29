# REM : la recherche de Mesquite est bien sur effectuee a partir de CMAKE_PREFIX_PATH mais le contenu des
# repertoires racines identifies par MESQUITE_DIR et Mesquite_ROOT est egalement evalue.
#
# The module defines the following variables:
#  MESQUITE_FOUND - the system has Mesquite
#  MESQUITE_INCLUDE_DIR - where to find Mesquite.hpp
#  MESQUITE_INCLUDE_DIRS - Mesquite includes
#  MESQUITE_LIBRARY - where to find the Mesquite library
#  MESQUITE_LIBRARIES - additional libraries
#  MESQUITE_ROOT_DIR - root dir


# message (STATUS "MESQUITE_INCLUDE_DIR: '${MESQUITE_INCLUDE_DIR}'")

message (STATUS "=====================================> MESQUITE_DIR: ${MESQUITE_DIR} Mesquite_ROOT=${Mesquite_ROOT}")

if (MESQUITE_DIR)
	find_path (MESQUITE_INCLUDE_DIR Mesquite.hpp PATHS ${MESQUITE_DIR}/include)
elseif (Mesquite_ROOT)
	find_path (MESQUITE_INCLUDE_DIR Mesquite.hpp PATHS ${Mesquite_ROOT}/include)
else()
	find_path (MESQUITE_INCLUDE_DIR Mesquite.hpp)
endif()
	
# message( "MESQUITE_INCLUDE_DIR: '${MESQUITE_INCLUDE_DIR}'" )

set (MESQUITE_INCLUDE_DIRS ${MESQUITE_INCLUDE_DIR} )

if (MESQUITE_DIR)
	find_library (MESQUITE_LIBRARY mesquite PATHS ${MESQUITE_DIR} PATH_SUFFIXES shlib64;shlib;lib64;lib)
elseif (Mesquite_ROOT)
	find_library (MESQUITE_LIBRARY mesquite PATHS ${Mesquite_ROOT} PATH_SUFFIXES shlib64;shlib;lib64;lib)
else()
	find_library (MESQUITE_LIBRARY mesquite)
endif()

if (MESQUITE_LIBRARY)
	get_filename_component (EXTENSION ${MESQUITE_LIBRARY} LAST_EXT)
endif ()

set (MESQUITE_LIBRARIES ${MESQUITE_LIBRARY})

# try to guess root dir from include dir
if (MESQUITE_INCLUDE_DIR)
  string ( REGEX REPLACE "(.*)/include.*" "\\1" MESQUITE_ROOT_DIR ${MESQUITE_INCLUDE_DIR} )
# try to guess root dir from library dir
elseif (MESQUITE_LIBRARY)
  string ( REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" MESQUITE_ROOT_DIR ${MESQUITE_LIBRARY} )
endif ()

mark_as_advanced (
  MESQUITE_LIBRARY
  MESQUITE_LIBRARIES
  MESQUITE_INCLUDE_DIR
  MESQUITE_INCLUDE_DIRS
  MESQUITE_ROOT_DIR
)

set (MESQUITE_TARGET "Mesquite::Mesquite")

if (EXTENSION STREQUAL ".a")
	add_library (${MESQUITE_TARGET} STATIC IMPORTED)
else()
	add_library (${MESQUITE_TARGET} SHARED IMPORTED)
endif ()

set_target_properties (Mesquite::Mesquite PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ${MESQUITE_INCLUDE_DIR}
		IMPORTED_LOCATION ${MESQUITE_LIBRARIES}
	)
