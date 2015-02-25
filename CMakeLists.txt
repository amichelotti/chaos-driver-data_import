cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(DataImport_src source/main.cpp source/DataImport.cpp source/DataImportDriver.cpp )

SET(DataImportClient_src test/DataImportClient.cpp )

ADD_EXECUTABLE(DataImport ${DataImport_src})
ADD_EXECUTABLE(DataImportClient ${DataImportClient_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
TARGET_LINK_LIBRARIES(DataImport chaos_cutoolkit ${FrameworkLib})
TARGET_LINK_LIBRARIES(DataImportClient chaos_uitoolkit ${FrameworkLib})
ELSE()
TARGET_LINK_LIBRARIES(DataImport chaos_cutoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(DataImportClient chaos_uitoolkit chaos_common ${FrameworkLib})
ENDIF()


INSTALL_TARGETS(/bin DataImport)
INSTALL_TARGETS(/bin DataImportClient)
