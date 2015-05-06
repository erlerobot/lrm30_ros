cmake_minimum_required(VERSION 2.8.3)
project(lrm30_ros)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs genmsg serial)

## Declare ROS messages and services
#add_message_files(DIRECTORY msg FILES )

## Generate added messages and services
generate_messages(DEPENDENCIES std_msgs)

## Declare a catkin package
catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(lrm30 src/lrm30.cpp src/LRM30_serial.cpp)
target_link_libraries(lrm30 ${catkin_LIBRARIES})
add_dependencies(lrm30 lrm30_listener_generate_messages_cpp)

add_executable(lrm30_listener src/lrm30_listener.cpp )
target_link_libraries(lrm30_listener ${catkin_LIBRARIES})
add_dependencies(lrm30_listener lrm30_listener_generate_messages_cpp)

file(GLOB SCRIPTS ${PROJECT_SOURCE_DIR}/examples/*)
  catkin_install_python(PROGRAMS
  ${SCRIPTS}
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
	TARGETS lrm30 lrm30_listener
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
