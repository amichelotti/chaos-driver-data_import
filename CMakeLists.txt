cmake_minimum_required(VERSION 2.8)
project(chaos_driver_data_import)
find_package(chaos MODULE REQUIRED)
include_directories(../../ ${chaos_INCLUDE_DIRS})


add_subdirectory(models)

SET(data_import_src
   core/AttributeOffLen.cpp
   core/DataImport.cpp
  core/AbstractDataImportDriver.cpp
  core/AbstractDataImportDriver.cpp
  core/DataImportDriverInterface.cpp 
  core/RTDB.cpp core/DataImportHistory.cpp)


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${data_import_src})

ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${data_import_src})
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_misc_data ${FrameworkLib} ${lib})

#INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
FILE(GLOB files "core/*.h")
INSTALL(FILES ${files} DESTINATION include/driver/data-import/core)

