#
#    Copyright 2012, 2013, 2014 Thomas Schöps
#    Copyright 2012-2016 Kai Pastor
#    
#    This file is part of OpenOrienteering.
# 
#    OpenOrienteering is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
# 
#    OpenOrienteering is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
 
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
cmake_policy(SET CMP0020 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FeatureSummary)

# Project declaration

project(Mapper)

set(Mapper_COPYRIGHT "(C) 2012-2015 OpenOrienteering developers")

# All version components must be integers.
# We use Mapper_VERSION_PATCH >= 99 to mark development builds.
set(Mapper_VERSION_MAJOR 0)
set(Mapper_VERSION_MINOR 6)
if(NOT Mapper_VERSION_PATCH)
	# This can be configured when running cmake
	set(Mapper_VERSION_PATCH 1)
endif()

set(MAPPER_VERSION_PRI "${PROJECT_SOURCE_DIR}/oo-mapper-version.pri")
file(READ "${MAPPER_VERSION_PRI}" current)
string(MD5 file_md5 "${current}")
set(output "# Generated in CMakeLists.txt, do not edit here.
Mapper_VERSION_MAJOR = ${Mapper_VERSION_MAJOR}
Mapper_VERSION_MINOR = ${Mapper_VERSION_MINOR}
Mapper_VERSION_PATCH = ${Mapper_VERSION_PATCH}
")
string(MD5 output_md5 "${output}")
if(NOT "${output_md5}" STREQUAL "${file_md5}")
	file(WRITE "${MAPPER_VERSION_PRI}" "${output}")
endif()

if(${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
	message(AUTHOR_WARNING "In-source builds are discouraged for development.")
endif()

# Build configuration options

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
	  "Type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
	  FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(APPLE)
	SET(Mapper_MACOSX_VERSION "10.7" CACHE STRING
	  "Minimum Mac OS X version to build for, recommended: 10.7")
endif()

option(Mapper_TRANSLATIONS_EMBEDDED "Embed all translations in the program executable")

option(Mapper_DEBUG_TRANSLATIONS "Debug missing translations" OFF)

set(Mapper_BUILD_CLIPPER_DEFAULT ON)
if(WIN32 OR APPLE)
	set(Mapper_BUILD_PROJ_DEFAULT ON)
	set(Mapper_BUILD_GDAL_DEFAULT ON)
	set(Mapper_BUILD_QT_DEFAULT ON)
	set(Mapper_BUILD_DOXYGEN_DEFAULT ON)
	set(Mapper_BUILD_PACKAGE_DEFAULT ON)
else()
	set(Mapper_BUILD_PROJ_DEFAULT OFF)
	set(Mapper_BUILD_GDAL_DEFAULT OFF)
	set(Mapper_BUILD_QT_DEFAULT ON)
	set(Mapper_BUILD_DOXYGEN_DEFAULT OFF)
	set(Mapper_BUILD_PACKAGE_DEFAULT OFF)
endif()

option(Mapper_BUILD_CLIPPER "Build the Clipper package from source" ${Mapper_BUILD_CLIPPER_DEFAULT})

option(Mapper_BUILD_PROJ "Build the Proj library from source" ${Mapper_BUILD_PROJ_DEFAULT})

option(Mapper_BUILD_GDAL "Build the GDAL library" ${Mapper_BUILD_GDAL_DEFAULT})

option(Mapper_USE_GDAL   "Use the GDAL library" ${Mapper_BUILD_GDAL})

option(Mapper_BUILD_QT   "Build the Qt library from source" ${Mapper_BUILD_QT_DEFAULT})

option(Mapper_BUILD_DOXYGEN "Build the doxygen tool from source" ${Mapper_BUILD_DOXYGEN_DEFAULT})

if(CMAKE_BUILD_TYPE MATCHES Release|MinSizeRel|RelWithDebInfo)
	set(Mapper_DEVELOPMENT_BUILD_DEFAULT OFF)
else()
	set(Mapper_DEVELOPMENT_BUILD_DEFAULT ON)
endif()
option(Mapper_DEVELOPMENT_BUILD "Configure development build (loading resource from the build directory)" ${Mapper_DEVELOPMENT_BUILD_DEFAULT})
mark_as_advanced(Mapper_DEVELOPMENT_BUILD)

option(Mapper_AUTORUN_UNIT_TESTS "Run the unit tests as part of the default target"   ${Mapper_DEVELOPMENT_BUILD})
option(Mapper_AUTORUN_SYSTEM_TESTS "Run the system tests as part of the default target" OFF)
mark_as_advanced(Mapper_AUTORUN_UNIT_TESTS Mapper_AUTORUN_SYSTEM_TESTS)

option(Mapper_BUILD_PACKAGE     "Create a target for building packages"                ${Mapper_BUILD_PACKAGE_DEFAULT})
option(Mapper_PACKAGE_PROJ      "Include all required Proj components in the packages" ${Mapper_BUILD_PROJ})
option(Mapper_PACKAGE_GDAL      "Include all required GDAL components in the packages" ${Mapper_BUILD_GDAL})
option(Mapper_PACKAGE_QT        "Include all required Qt components in the packages"   ${Mapper_BUILD_QT})
option(Mapper_PACKAGE_ASSISTANT "Include Qt Assistant in the packages"                 ${Mapper_BUILD_QT})
option(Mapper_PACKAGE_LINGUIST  "Build a Qt Linguist package"                          ${Mapper_BUILD_PACKAGE_DEFAULT})
mark_as_advanced(Mapper_BUILD_PACKAGE Mapper_PACKAGE_QT Mapper_PACKAGE_ASSISTANT Mapper_PACKAGE_PROJ Mapper_PACKAGE_LINGUIST)

unset(Mapper_PACKAGE_LIBRARIES CACHE) # Legacy


# Installation configuration

set(Mapper_PACKAGE_NAME "openorienteering-mapper" CACHE STRING
  "The package name"
)

# These value are used for the DESTINATION parameter of the install command
# and must not be empty.
if(WIN32 AND BIN_INSTALL_DIR)
	set(MAPPER_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}")
	set(MAPPER_LIBRARY_DESTINATION "${LIB_INSTALL_DIR}")
	set(MAPPER_DATA_DESTINATION    "${SHARE_INSTALL_DIR}/${Mapper_PACKAGE_NAME}")
	set(MAPPER_ABOUT_DESTINATION   "${SHARE_INSTALL_DIR}/doc/${Mapper_PACKAGE_NAME}")
elseif(WIN32)
	set(MAPPER_RUNTIME_DESTINATION .)
	set(MAPPER_LIBRARY_DESTINATION .)
	set(MAPPER_DATA_DESTINATION    .)
	set(MAPPER_ABOUT_DESTINATION   "doc")
elseif(APPLE)
	set(MAPPER_RUNTIME_DESTINATION .)
	set(MAPPER_LIBRARY_DESTINATION .)
	set(MAPPER_DATA_DESTINATION    "Mapper.app/Contents/Resources")
	set(MAPPER_ABOUT_DESTINATION   "Mapper.app/Contents/Resources")
else() # LINUX and alike
	set(MAPPER_RUNTIME_DESTINATION "/usr/bin")
	set(MAPPER_LIBRARY_DESTINATION "/usr/lib/${Mapper_PACKAGE_NAME}")
	set(MAPPER_DATA_DESTINATION    "/usr/share/${Mapper_PACKAGE_NAME}")
	set(MAPPER_ABOUT_DESTINATION   "/usr/share/doc/${Mapper_PACKAGE_NAME}")
endif()

if(CMAKE_CROSSCOMPILING)
	message(STATUS "Crosscompiling, host: ${CMAKE_HOST_SYSTEM_NAME}, target: ${CMAKE_SYSTEM_NAME}")
endif()


# Build definitons

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" std_cxx11)
if(NOT std_cxx11)
	check_cxx_compiler_flag("-std=c++0x" std_cxx0x)
endif()
if(std_cxx11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(std_cxx0x)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(CMAKE_COMPILER_IS_GNUXX)
	message(FATAL_ERROR "The compiler does not support C++11.")
else()
	message(FATAL_ERROR "The compiler support for C++11 is not known.")
endif()

if(APPLE AND Mapper_MACOSX_VERSION)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -mmacosx-version-min=${Mapper_MACOSX_VERSION}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=${Mapper_MACOSX_VERSION}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=${Mapper_MACOSX_VERSION}")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wpedantic")
endif()

add_custom_target(Mapper_prerequisites
  SOURCES # Extra files to be shown in IDE
    INSTALL.md
    README.md
    oo-mapper.pro
    ${MAPPER_VERSION_PRI}
)
set(Mapper_prerequisites_FOUND TRUE)

if(Mapper_BUILD_CLIPPER)
	add_subdirectory(3rd-party/clipper)
	add_feature_info(Mapper_BUILD_CLIPPER 1 "version: ${CLIPPER_VERSION}")
else()
	set(CLIPPER_VERSION 6.1.3a)
	find_package(Clipper MODULE REQUIRED)
endif()

if(Mapper_BUILD_PROJ)
	add_subdirectory(3rd-party/proj)
	add_dependencies(Mapper_prerequisites Proj)
	add_feature_info(Mapper_BUILD_PROJ 1 "version: ${PROJ_VERSION}")
else()
	find_package(Proj REQUIRED)
endif()

if(Mapper_BUILD_GDAL)
	add_subdirectory(3rd-party/gdal)
	add_dependencies(Mapper_prerequisites GDAL)
	if(TARGET Proj)
		add_dependencies(GDAL Proj)
	endif()
	if(NOT GDAL_FOUND)
		set(Mapper_prerequisites_FOUND FALSE)
	endif()
	add_feature_info(Mapper_BUILD_GDAL 1 "version: ${GDAL_VERSION}")
elseif(Mapper_USE_GDAL)
	find_package(GDAL REQUIRED)
endif()

if(Mapper_BUILD_QT)
	add_subdirectory(3rd-party/qt5)
	add_dependencies(Mapper_prerequisites Qt5)
	list(INSERT CMAKE_PREFIX_PATH 0 "${QT5_DIR}")
	find_package(Qt5Core QUIET PATHS "${QT5_DIR}" NO_DEFAULT_PATH)
	if(NOT Qt5Core_FOUND)
		set(Mapper_prerequisites_FOUND FALSE)
	endif()
	add_feature_info(Mapper_BUILD_QT 1 "version: ${QT5_VERSION}")
endif()

if(Mapper_BUILD_DOXYGEN)
	add_subdirectory(3rd-party/doxygen)
	add_feature_info(Mapper_BUILD_DOXYGEN 1 "version: ${DOXYGEN_VERSION}")
endif()

if(Mapper_prerequisites_FOUND)

if(UNIX AND NOT APPLE)
#	set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib/${Mapper_PACKAGE_NAME}/lib")
	set(CMAKE_INSTALL_RPATH "${MAPPER_LIBRARY_DESTINATION}/lib")
endif()

add_definitions(-D_USE_MATH_DEFINES -DUNICODE)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PROJ_INCLUDE_DIR})

if(Mapper_DEVELOPMENT_BUILD)
	add_definitions(-DMAPPER_DEVELOPMENT_BUILD)
	include(EnableSanitize)
	enable_sanitize(address undefined)
	configure_file(suppress.txt.in suppress.txt COPY_ONLY)
else()
	add_definitions(-DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT -DNDEBUG)
endif()

if(NOT Mapper_VERSION_PATCH LESS 99)
	set(Mapper_VERSION_DISPLAY "unstable ${Mapper_VERSION_MAJOR}.${Mapper_VERSION_MINOR}.${Mapper_VERSION_PATCH}")
else()
	set(Mapper_VERSION_DISPLAY "${Mapper_VERSION_MAJOR}.${Mapper_VERSION_MINOR}.${Mapper_VERSION_PATCH}")
endif()

# Subdirectories

add_subdirectory("doc/licensing")
add_subdirectory("doc/manual")
add_subdirectory("examples")
add_subdirectory("symbol sets")
add_subdirectory("translations")
add_subdirectory("3rd-party/qbezier")
add_subdirectory("3rd-party/qtsingleapplication")
if (Mapper_USE_GDAL)
	add_subdirectory("src/gdal")
endif()
add_subdirectory("src/libocad")
add_subdirectory("src/printsupport")
add_subdirectory("src")
add_subdirectory("packaging")
if(CMAKE_CROSSCOMPILING)
	add_custom_target(TEST_WARNING ALL
	  COMMENT "Crosscompiling, skipping all tests")
	add_dependencies(TEST_WARNING Mapper)
else()
	enable_testing()
	add_subdirectory("test")
endif()

foreach(qt5_module Core Gui Network PrintSupport Test Widgets Xml)
	if (TARGET Qt5::${qt5_module} AND TARGET Qt5)
		add_dependencies(Qt5::${qt5_module} Qt5)
	endif()
endforeach()

install(FILES COPYING 
  DESTINATION "${MAPPER_ABOUT_DESTINATION}"
)
configure_file(COPYING COPYING COPYONLY)

set(MAPPER_SUPERBUILD 0 CACHE INTERNAL "Flag to avoid superbuild loops." FORCE)

elseif(MAPPER_SUPERBUILD)

	set(MAPPER_SUPERBUILD 0 CACHE INTERNAL "Flag to avoid superbuild loops." FORCE)
	message(FATAL_ERROR "Mapper prerequisites marked as build but not found by cmake re-run.")

else() # NOT Mapper_prerequisites_FOUND AND NOT MAPPER_SUPERBUILD

	if("${CMAKE_GENERATOR}" MATCHES "Makefiles" OR 
	   "${CMAKE_EXTRA_GENERATOR}" MATCHES "Makefiles")
		# use magic MAKE macro
		set(_build_tool "\$(MAKE)")
	else()
		set(_build_tool ${CMAKE_BUILD_TOOL})
	endif()
	add_custom_target(Mapper_superbuild ALL
	  COMMAND "${CMAKE_COMMAND}" . -DMAPPER_SUPERBUILD=1
	  COMMAND ${_build_tool}
	  VERBATIM
	)
	add_dependencies(Mapper_superbuild Mapper_prerequisites)

endif()

add_subdirectory("doc/api")
add_subdirectory("packaging/src")

feature_summary(WHAT ALL)

string(TOUPPER "${CMAKE_BUILD_TYPE}" type)
foreach(lang CXX C)
	foreach(i "" _${type})
		message(STATUS "CMAKE_${lang}_FLAGS${i}: ${CMAKE_${lang}_FLAGS${i}}")
	endforeach()
endforeach()
