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

# ADD_EXECUTABLE(memcacheUS test/main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} memcached pthread chaos_cutoolkit chaos_driver_data_import chaos_driver_misc chaos_common boost_random ${FrameworkLib})
# TARGET_LINK_LIBRARIES(memcacheUS ${PROJECT_NAME} chaos_driver_data_import chaos_driver_data_import_dante memcached pthread chaos_cutoolkit chaos_common boost_random ${FrameworkLib})
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)
FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/driver/data-import/models/memcache)


INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
#INSTALL_TARGETS(/bin memcacheUS)
#INSTALL(TARGETS memcacheUS
#        DESTINATION "bin"
#        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)