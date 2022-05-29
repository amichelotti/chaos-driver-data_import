cmake_minimum_required(VERSION 2.9)
IF (ENABLE_MEMCACHE)
  MESG(STATUS "memcache enabled")
  FILE(REMOVE memcache/UnitServerIgnore)
  FILE(REMOVE dante/UnitServerIgnore)

  add_subdirectory(memcache)
  add_subdirectory(dante)
ELSE()
  MESG(STATUS "memcache disabled")
  FILE(WRITE memcache/UnitServerIgnore "disabled ${PROJECT_NAME}")
  FILE(WRITE dante/UnitServerIgnore "disabled ${PROJECT_NAME}")

ENDIF()
add_subdirectory(posixFile)

SET(DRIVERS_LIB ${DRIVERS_LIB} PARENT_SCOPE)
