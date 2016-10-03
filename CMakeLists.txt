cmake_minimum_required(VERSION 2.8)
project(chaos_data_import)

ADD_DEFINITIONS(-std=c++11)
IF (ENABLE_MEMCACHE)
  MESG(STATUS "memcache enabled")
  SET(data_import_src   core/DataImport.cpp
                      core/AbstractDataImportDriver.cpp
                      core/DataImportDriverInterface.cpp 
		      core/RTChaos2Memcache.cpp 
		      core/RTDB.cpp core/DataImportHistory.cpp
		      models/MemcachedDataImporterDriver.cpp 
		      models/FilePosixDataImporterDriver.cpp)
		    FILE(REMOVE UnitServerIgnore)
		    ADD_EXECUTABLE(DataImportControlUnit ${data_import_lient_src})
		    TARGET_LINK_LIBRARIES(DataImportControlUnit ${PROJECT_NAME} chaos_cutoolkit chaos_common chaos_driver_misc common_misc_data ${FrameworkLib} memcached)
		    INSTALL_TARGETS(/bin DataImportControlUnit)
ELSE()
  MESG(STATUS "memcache disabled")
  FILE(WRITE UnitServerIgnore "disabled ${PROJECT_NAME}")
  SET(data_import_src   core/DataImport.cpp
                      core/AbstractDataImportDriver.cpp
                      core/DataImportDriverInterface.cpp 
		      core/RTDB.cpp core/DataImportHistory.cpp
		      models/FilePosixDataImporterDriver.cpp)
		    SET(data_import_lient_src source/main.cpp )


ENDIF()


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(${PROJECT_NAME} STATIC ${data_import_src})

ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${data_import_src})

ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common chaos_driver_misc common_misc_data ${FrameworkLib} memcached)



INSTALL_TARGETS(/lib ${PROJECT_NAME})
