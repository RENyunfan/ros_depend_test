cmake_minimum_required(VERSION 2.8.3)
project(complex_test)

find_package(catkin REQUIRED COMPONENTS
        roscpp
	my_math_lib
        )

catkin_package(
	CATKIN_DEPENDS  my_math_lib
)

include_directories(
	${catkin_INCLUDE_DIRS}
)

add_executable(main
        src/main.cpp
        )
target_link_libraries( main
        ${catkin_LIBRARIES}
        )

