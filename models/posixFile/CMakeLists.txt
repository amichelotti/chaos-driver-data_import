cmake_minimum_required(VERSION 2.8)
project(chaos_driver_data_import_posixFile)

SET(lib FilePosixDataImporterDriver.cpp)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${lib})
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${lib})
ENDIF()


TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_driver_data_import pthread chaos_cutoolkit chaos_common boost_random ${FrameworkLib})
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)
installHeaders(.)
INSTALL_TARGETS(/lib ${PROJECT_NAME})
