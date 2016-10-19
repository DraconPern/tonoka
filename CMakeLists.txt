cmake_minimum_required(VERSION 2.8)

project(tonoka)

set(VERSION 1.0.1)

add_definitions(-DTONOKA_VERSION="${VERSION}")

set (CMAKE_CXX_STANDARD 11)

if(WIN32)
	#VC++ stuff
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-D_UNICODE)
	add_definitions(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd -DDEBUG")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_release} /DEBUG")
endif()

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)
find_package(Boost REQUIRED thread filesystem system regex date_time)

set(wxWidgets_USE_STATIC   ON)
#define this so that wxWidgets doesn't define it. Conflicts w/ dcmtk's declaration
add_definitions(-DHAVE_SSIZE_T)
find_package(wxWidgets COMPONENTS core base adv REQUIRED)
include(${wxWidgets_USE_FILE})

if(WIN32)
	#enable dcmtk filename to have wide version, only affects WIN32.  http://utf8everywhere.org/
	add_definitions(-DDCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS=1)
endif(WIN32)
find_package(DCMTK REQUIRED CONFIG HINTS ${DCMTK_DIR})

#include zlib
find_package(ZLIB REQUIRED ${ZLIB_ROOT})

#include openjpeg
find_package(OpenJPEG REQUIRED HINTS ${OPENJPEG})
add_definitions(-DOPJ_STATIC)

#include fmjpeg2koj
find_package(fmjpeg2k REQUIRED HINTS ${FMJPEG2K})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/sqlite3 ${Boost_INCLUDE_DIRS} ${FMJPEG2K_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIRS} ${VLD}/include)
LINK_DIRECTORIES(${FMJPEG2K}/lib ${Boost_LIBRARY_DIRS}  ${VLD}/lib/Win32)

set(tonoka_HEADERS
	src/gui.h
	src/tonoka_about.h
	src/tonoka_destination.h
	src/tonoka_mainFrame.h
	src/tonoka_searchStatus.h
	src/tonoka_sendStatus.h
	src/tonoka_updateCheck.h
	sqlite3/sqlite3.h
	src/engine.h
	src/patientdata.h
	src/destinationentry.h
	src/dicomscanner.h
	src/dicomsender.h
	src/dcm2img.h
	src/update.h
	src/alphanum.hpp
)

set(tonoka_SRCS
  ${tonoka_HEADERS}
	src/main.cpp
	src/gui.cpp
	src/tonoka_about.cpp
	src/tonoka_destination.cpp
	src/tonoka_mainFrame.cpp
	src/tonoka_searchStatus.cpp
	src/tonoka_sendStatus.cpp
	src/tonoka_updateCheck.cpp
	sqlite3/sqlite3.c
	sqlite3/sqlite3_exec_stmt.c
	src/engine.cpp
	src/patientdata.cpp
	src/dicomscanner.cpp
	src/dicomsender.cpp
	src/dcm2img.cpp
	src/update.cpp
)

if(WIN32)
  set(tonoka_SRCS ${tonoka_SRCS} src/win/tonoka.rc src/win/version.rc)

  string(REPLACE "." "," VER_FILEVERSION ${VERSION})

  add_definitions(-DVER_FILEVERSION=${VER_FILEVERSION})
  add_definitions(-DVER_FILEVERSION_STR="${VERSION}")
  add_definitions(-DVER_PRODUCTVERSION=${VER_FILEVERSION})
  add_definitions(-DVER_PRODUCTVERSION_STR="${VERSION}")

endif()

if(APPLE)
  set(tonoka_SRCS ${tonoka_SRCS} src/osx/tonoka.icns src/osx/Info.plist.in)
endif()

ADD_EXECUTABLE(tonoka WIN32 MACOSX_BUNDLE ${tonoka_SRCS})

IF(WIN32)
  SET(OS_SPECIFIC_LIBS netapi32 wsock32 odbc32 Rpcrt4)
ELSE()
  IF(APPLE)
  ELSE()
    SET(OS_SPECIFIC_LIBS dl)
  ENDIF()
ENDIF()

if(APPLE)
  set_target_properties(tonoka PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/osx/Info.plist.in)
  set_target_properties(tonoka PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/src/osx/tonoka.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF()

TARGET_LINK_LIBRARIES(tonoka
	${FMJPEG2K_LIBRARIES}
	${OPENJPEG_LIBRARIES}
	${ZLIB_LIBRARIES}
	${DCMTK_LIBRARIES}
	${wxWidgets_LIBRARIES}
	${Boost_LIBRARIES}
	${OS_SPECIFIC_LIBS}
	)
