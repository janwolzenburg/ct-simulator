cmake_minimum_required(VERSION 3.28)

project( ct-simulator )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

add_compile_options( -W3  )
add_compile_definitions( _CONSOLE )

FILE(GLOB SOURCES ${CMAKE_SOURCE_DIR}./*.cpp)
add_executable( ct-simulator ${SOURCES} )

include_directories( ${CMAKE_SOURCE_DIR}/lib/include )
link_directories( ${CMAKE_SOURCE_DIR}/lib )

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    target_link_libraries( ct-simulator fltk fltk_images fltk_png fltk_z )
    add_compile_definitions( _NDEBUG)
elseif( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    target_link_libraries( ct-simulator fltkd fltk_imagesd fltk_pngd fltk_zd )
    add_compile_definitions( _DEBUG)
endif()