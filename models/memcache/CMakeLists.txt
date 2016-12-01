cmake_minimum_required(VERSION 2.8)
project(chaos_driver_data_import_memcache)
ADD_DEFINITIONS(-std=c++11)

SET(lib MemcachedDataImporterDriver.cpp RTChaos2Memcache.cpp)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${lib})
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${lib})
ENDIF()

ADD_EXECUTABLE(memcacheImport test/main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} memcached pthread chaos_cutoolkit chaos_common boost_random ${FrameworkLib})
TARGET_LINK_LIBRARIES(memcacheImport ${PROJECT_NAME} chaos_driver_data_import memcached pthread chaos_cutoolkit chaos_common boost_random ${FrameworkLib})
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)
installHeaders(.)
INSTALL_TARGETS(/lib ${PROJECT_NAME})
