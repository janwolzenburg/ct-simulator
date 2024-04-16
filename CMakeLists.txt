cmake_minimum_required(VERSION 3.28)

project( ct-simulator )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

add_compile_options(-W3 -MP)
add_compile_definitions(_CONSOLE WIN32) # Remove WIN32 when not on windows platform

FILE(GLOB SOURCES ${CMAKE_SOURCE_DIR}./*.cpp)
add_executable( ct-simulator ${SOURCES} )

target_include_directories( ct-simulator PUBLIC ${CMAKE_SOURCE_DIR}/lib/include )
target_link_directories( ct-simulator PUBLIC ${CMAKE_SOURCE_DIR}/lib )

target_link_libraries( ct-simulator debug fltkd         optimized fltk 
                                    debug fltk_imagesd  optimized fltk_images
                                    debug fltk_pngd     optimized fltk_png 
                                    debug fltk_zd       optimized fltk_z )                              