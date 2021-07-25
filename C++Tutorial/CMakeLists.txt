cmake_minimum_required ( VERSION 3.5 )

project ( c++Tutorial )

set ( CMKAE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14" )

AUX_SOURCE_DIRECTORY( src DIR_SRCS)
INCLUDE_DIRECTORIES ( inc )
add_executable ( emptyProj      src/emptyProj.cpp  )
add_executable ( logClass       src/logClass.cpp   )
add_executable ( SimpleClass    src/SimpleClass.cpp)
add_executable ( Inheritance     src/Inheritance.cpp )