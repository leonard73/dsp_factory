cmake_minimum_required ( VERSION 3.5 )
project ( c++Tutorial )
set ( CMKAE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14" )
AUX_SOURCE_DIRECTORY( src DIR_SRCS)
INCLUDE_DIRECTORIES ( inc )
add_executable ( emptyProj              src/emptyProj.cpp           )
add_executable ( logClass               src/logClass.cpp            )
add_executable ( Inheritance            src/Inheritance.cpp         )
add_executable ( ConstKeyword           src/ConstKeyword.cpp        )
add_executable ( InitialMembers         src/InitialMembers.cpp      )
add_executable ( CreateInstanceStack    src/CreateInstanceStack.cpp )
add_executable ( NewAndMalloc           src/NewAndMalloc.cpp        )  
add_executable ( ExplicitUsage          src/ExplicitUsage.cpp       )  
add_executable ( Operator_Struct        src/Operator_Struct.cpp     )  
add_executable ( UniqueShareWeak        src/UniqueShareWeak.cpp     )  
add_executable ( MultipleReturns        src/MultipleReturns.cpp     )  
add_executable ( Macro                  src/Macro.cpp               )  
add_executable ( FunctionPointer        src/FunctionPointer.cpp     )  
