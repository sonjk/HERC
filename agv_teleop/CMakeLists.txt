cmake_minimum_required(VERSION 2.8.3)
project(agv_teleop)

find_package(catkin REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  serial
  std_msgs
  geometry_msgs
  joy
  sensor_msgs
  nav_msgs
  tf
  agv_msgs
)


catkin_package(
    INCLUDE_DIRS include
    CATKIN_DEPENDS roscpp std_msgs sensor_msgs geometry_msgs nav_msgs tf
)

include_directories(
    include
    ${catkin_INCLUDE_DIRS}
)

link_directories(
    ${catkin_LIBRARIES}
    )



add_executable(agv_teleop_diff_node src/agv_teleop_diff.cpp)
target_link_libraries(agv_teleop_diff_node ${catkin_LIBRARIES})


#############
## Install ##
#############

## Mark executables and/or libraries for installation

install(TARGETS agv_teleop_diff_node
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
    PATTERN ".svn" EXCLUDE
    )


