cmake_minimum_required(VERSION 3.6)

project( ct-simulator )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

add_compile_options(-Wall -Wno-sign-compare -Wno-reorder -Wno-misleading-indentation -fpermissive -MP
-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
)
add_link_options(  )
#add_compile_definitions(_CONSOLE WIN32) # Remove WIN32 when not on windows platform

FILE(GLOB SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)
add_executable( ct-simulator ${SOURCES} )

target_include_directories( ct-simulator PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( ct-simulator PUBLIC ${CMAKE_SOURCE_DIR}/lib/include )
target_include_directories( ct-simulator PUBLIC /usr/local/include/FL/images  )
target_include_directories( ct-simulator PUBLIC /usr/include/freetype2 )
target_include_directories( ct-simulator PUBLIC /usr/include/libpng16 )

#target_link_directories( ct-simulator PUBLIC ${CMAKE_SOURCE_DIR}/lib )

target_link_libraries( ct-simulator fltk.a fltk_png.a fltk_z.a fltk_images.a png Xrender Xft X11 fontconfig pthread m)                              
