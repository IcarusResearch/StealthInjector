cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "StealthInjector")

project(${PROJECT_NAME} CXX ASM_MASM)

set(SI_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")

add_definitions(-DUNICODE -D_UNICODE)
add_compile_definitions(SI_COMPILE)

message(STATUS "Building ${PROJECT_NAME}...")

file(GLOB_RECURSE SI_SOURCES
	"${SI_SOURCES_DIR}/**.cpp"
	"${SI_SOURCES_DIR}/**.h"
	"${SI_SOURCES_DIR}/**.asm"
)

foreach(FILE ${SI_SOURCES})
  message(STATUS "Including file: ${FILE}")
endforeach()

add_executable(${PROJECT_NAME} ${SI_SOURCES})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
target_include_directories(${PROJECT_NAME} PRIVATE
	"${SI_SOURCES_DIR}"
	"${PROJECT_SOURCE_DIR}/Winternals"
	"${PROJECT_SOURCE_DIR}/WIL/include/wil"
)
