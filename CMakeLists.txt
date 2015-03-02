cmake_minimum_required(VERSION 2.6)
project(data_import)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(data_import_src   core/DataImport.cpp
                      core/AbstractDataImportDriver.cpp
                      core/DataImportDriverInterface.cpp)
SET(data_import_src   ${data_import_src}
                      models/MemcachedDataImporterDriver.cpp)

SET(data_import_lient_src test/main.cpp )

ADD_EXECUTABLE(DataImportControlUnit ${data_import_lient_src})
IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(chaos_data_import STATIC ${data_import_src})

ELSE()
    ADD_LIBRARY(chaos_data_import SHARED ${data_import_src})

ENDIF()
TARGET_LINK_LIBRARIES(chaos_data_import chaos_cutoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(DataImportControlUnit chaos_data_import chaos_cutoolkit chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin DataImportControlUnit)
INSTALL_TARGETS(/lib chaos_data_import)
