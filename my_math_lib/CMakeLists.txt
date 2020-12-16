cmake_minimum_required(VERSION 2.8.3)
project(my_math_lib)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        )


catkin_package(
	INCLUDE_DIRS include
        LIBRARIES my_math_lib
)

include_directories(
	#SYSTEM
	include 
        ${catkin_INCLUDE_DIRS}

)

add_library(my_math_lib
        src/my_math_lib.cpp
        )
target_link_libraries( my_math_lib
        ${catkin_LIBRARIES}
        )

