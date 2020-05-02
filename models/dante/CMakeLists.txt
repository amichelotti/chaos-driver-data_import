cmake_minimum_required(VERSION 2.8)
project(chaos_driver_data_import_dante)
SET(lib DanteDriver.cpp)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -lcommon-crest-Wl,--no-whole-archive")
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${lib})
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${lib})
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} memcached pthread chaos_cutoolkit chaos_driver_data_import chaos_common boost_random common-crest chaos_driver_data_import_memcache chaos_driver_data_import ${FrameworkLib})
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)
FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/driver/data-import/models/dante)

INSTALL_TARGETS(/lib ${PROJECT_NAME})
