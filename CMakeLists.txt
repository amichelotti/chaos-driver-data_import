cmake_minimum_required(VERSION 2.6)
project(chaos_data_import)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ADD_DEFINITIONS(-std=c++11)

SET(data_import_src   core/DataImport.cpp
                      core/AbstractDataImportDriver.cpp
                      core/DataImportDriverInterface.cpp core/RTChaos2Memcache.cpp core/RTDB.cpp core/DataImportHistory.cpp)
SET(data_import_src   ${data_import_src}
                      models/MemcachedDataImporterDriver.cpp models/FilePosixDataImporterDriver.cpp)

SET(data_import_lient_src source/main.cpp )

ADD_EXECUTABLE(DataImportControlUnit ${data_import_lient_src})
IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(${PROJECT_NAME} STATIC ${data_import_src})

ELSE()
    ADD_LIBRARY(${PROJECT_NAME} SHARED ${data_import_src})

ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common chaos_driver_misc common_misc_data ${FrameworkLib} memcached)
TARGET_LINK_LIBRARIES(DataImportControlUnit ${PROJECT_NAME} chaos_cutoolkit chaos_common chaos_driver_misc common_misc_data ${FrameworkLib} memcached)

INSTALL_TARGETS(/bin DataImportControlUnit)
INSTALL_TARGETS(/lib ${PROJECT_NAME})
