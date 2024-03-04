cmake_minimum_required(VERSION 3.8)
project(astra_camera)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -fPIC -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -g")
set(CMAKE_BUILD_TYPE "Release")
option(USE_RK_HW_DECODER "Use Rockchip hardware decoder" OFF)
option(USE_NV_HW_DECODER "Use Nvidia hardware decoder" OFF)
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(astra_camera_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(camera_info_manager REQUIRED)
find_package(class_loader REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(message_filters REQUIRED)
find_package(camera_info_manager REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(image_geometry REQUIRED)
find_package(image_publisher REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rcutils REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_sensor_msgs REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(LIBUVC REQUIRED libuvc)
if (NOT LIBUVC_FOUND)
  message(FATAL_ERROR "libuvc is not found")
endif ()
pkg_search_module(GLOG REQUIRED libglog)

if (NOT GLOG_FOUND)
  message(FATAL_ERROR "glog is not found")
endif ()

execute_process(COMMAND uname -m OUTPUT_VARIABLE MACHINES)
execute_process(COMMAND getconf LONG_BIT OUTPUT_VARIABLE MACHINES_BIT)
message(STATUS "ORRBEC Machine : ${MACHINES}")
message(STATUS "ORRBEC Machine Bits : ${MACHINES_BIT}")

if ((${MACHINES} MATCHES "x86_64") AND (${MACHINES_BIT} MATCHES "64"))
  set(HOST_PLATFORM "x64")
elseif (${MACHINES} MATCHES "arm")
  set(HOST_PLATFORM "arm")
elseif ((${MACHINES} MATCHES "aarch64") AND (${MACHINES_BIT} MATCHES "64"))
  set(HOST_PLATFORM "arm64")
elseif ((${MACHINES} MATCHES "aarch64") AND (${MACHINES_BIT} MATCHES "32"))
  set(HOST_PLATFORM "arm")
endif ()

message(STATUS "ORRBEC : ${HOST_PLATFORM}")
set(ORBBEC_OPENNI2_REDIST
  "${CMAKE_CURRENT_SOURCE_DIR}/openni2_redist/${HOST_PLATFORM}")

message(STATUS "${LIB_NAME} Ros version : $ENV{ROS_DISTRO}")
if ($ENV{ROS_DISTRO} MATCHES "iron")
  add_definitions(-DUSE_IRON_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "humble")
  add_definitions(-DUSE_HUMBLE_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "galactic")
  add_definitions(-DUSE_GALACTIC_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "foxy")
  add_definitions(-DUSE_FOXY_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "eloquent")
  add_definitions(-DUSE_ELOQUENT_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "dashing")
  add_definitions(-DUSE_DASHING_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "noetic")
  add_definitions(-DUSE_ROS_NORTIC_VERSION)
elseif ($ENV{ROS_DISTRO} MATCHES "melodic")
  add_definitions(-DUSE_ROS_MELODIC_VERSION)
endif()

if (USE_RK_HW_DECODER)
  pkg_search_module(RK_MPP REQUIRED rockchip_mpp)
  if (NOT RK_MPP_FOUND)
    message(FATAL_ERROR "rockchip_mpp is not found")
  endif ()
  pkg_search_module(RGA librga)
  if (NOT RGA_FOUND)
    add_definitions(-DUSE_LIBYUV)
    message("librga is not found, use libyuv instead")
  endif ()
endif ()

if (USE_NV_HW_DECODER)
  set(JETSON_MULTI_MEDIA_API_DIR /usr/src/jetson_multimedia_api)
  set(JETSON_MULTI_MEDIA_API_CLASS_DIR ${JETSON_MULTI_MEDIA_API_DIR}/samples/common/classes)
  set(JETSON_MULTI_MEDIA_API_INCLUDE_DIR ${JETSON_MULTI_MEDIA_API_DIR}/include/)
  set(LIBJPEG8B_INCLUDE_DIR ${JETSON_MULTI_MEDIA_API_INCLUDE_DIR}/libjpeg-8b)
  set(TEGRA_ARMABI /usr/lib/aarch64-linux-gnu/)
  add_definitions(-DUSE_NV_HW_DECODER)
  add_compile_options(-Wno-missing-field-initializers -Wno-unused-parameter)
  set(NV_LIBRARIES
    -lnvjpeg -lnvbufsurface -lnvbufsurftransform -lyuv -lv4l2
  )
  list(APPEND NV_LIBRARIES
    -L${TEGRA_ARMABI} -L${TEGRA_ARMABI}/tegra)
endif ()

if (USE_RK_HW_DECODER)
  list(APPEND COMMON_LIBRARIES
    ${RK_MPP_LIBRARIES}
    ${RGA_LIBRARIES}
  )
endif ()

if (USE_NV_HW_DECODER)
  list(APPEND COMMON_LIBRARIES
    ${NV_LIBRARIES})
endif ()

set(dependencies
  ament_index_cpp
  astra_camera_msgs
  builtin_interfaces
  class_loader
  cv_bridge
  Eigen3
  image_geometry
  image_publisher
  image_transport
  rclcpp
  rclcpp_components
  rcutils
  sensor_msgs
  std_msgs
  std_srvs
  tf2
  tf2_msgs
  tf2_ros
  tf2_sensor_msgs
  message_filters
  camera_info_manager
  )

add_library(${PROJECT_NAME} SHARED
  src/point_cloud_proc/point_cloud_xyz.cpp
  src/point_cloud_proc/point_cloud_xyzrgb.cpp
  src/device_listener.cpp
  src/dynamic_params.cpp
  src/ob_camera_info.cpp
  src/ob_frame_listener.cpp
  src/ob_timer_filter.cpp
  src/ob_camera_node_factory.cpp
  src/ob_camera_node.cpp
  src/ros_param_backend.cpp
  src/ros_service.cpp
  src/utils.cpp
  src/ob_context.cpp
  src/uvc_camera_driver.cpp
  src/clean_up_shm_node.cpp
  )

if (USE_RK_HW_DECODER)
  add_definitions(-DUSE_RK_HW_DECODER)
  list(APPEND SOURCE_FILES src/rk_mpp_decoder.cpp)
  list(APPEND COMMON_INCLUDE_DIRS
    ${RK_MPP_INCLUDE_DIRS}
    ${RGA_INCLUDE_DIRS}
  )
  list(APPEND COMMON_LIBRARIES
    ${RGA_LIBRARIES}
    ${RK_MPP_LIBRARIES}
  )
  if (NOT RGA_FOUND)
    list(APPEND COMMON_LIBRARIES
      -lyuv
    )
  endif ()
endif ()

if (USE_NV_HW_DECODER)
  list(APPEND SOURCE_FILES src/jetson_nv_decoder.cpp)
  list(APPEND COMMON_INCLUDE_DIRS ${JETSON_MULTI_MEDIA_API_INCLUDE_DIR}
    ${LIBJPEG8B_INCLUDE_DIR})
  # append jetson_multimedia_api source files
  list(APPEND SOURCE_FILES
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvBuffer.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvElement.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvElementProfiler.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvJpegDecoder.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvJpegEncoder.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvLogging.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvUtils.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvV4l2Element.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvV4l2ElementPlane.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvVideoDecoder.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvVideoEncoder.cpp
    ${JETSON_MULTI_MEDIA_API_CLASS_DIR}/NvBufSurface.cpp
  )
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDED_DIRS}
  ${GLOG_INCLUDED_DIRS}
  ${libuvc_INCLUDE_DIRS}
  )

ament_target_dependencies(${PROJECT_NAME}
  ${dependencies}
  )
target_link_libraries(${PROJECT_NAME}
  ${OpenCV_LIBS}
  Eigen3::Eigen
  ${GLOG_LIBRARIES}
  ${LIBUVC_LIBRARIES}
  -lOpenNI2
  -L${ORBBEC_OPENNI2_REDIST}
  )

add_executable(list_devices_node
  src/list_devices_node.cpp
  )
target_include_directories(list_devices_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDED_DIRS}
  ${libuvc_INCLUDE_DIRS}
  )

ament_target_dependencies(list_devices_node
  ${dependencies}
  )
target_link_libraries(list_devices_node
  ${OpenCV_LIBS}
  Eigen3::Eigen
  ${GLOG_LIBRARIES}
  ${LIBUVC_LIBRARIES}
  -lOpenNI2
  -L${ORBBEC_OPENNI2_REDIST}
  ${PROJECT_NAME}
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE "COMPOSITION_BUILDING_DLL")

rclcpp_components_register_nodes(${PROJECT_NAME}
  "${PROJECT_NAME}::PointCloudXyzrgbNode"
  "${PROJECT_NAME}::PointCloudXyzNode"
  "${PROJECT_NAME}::OBCameraNodeFactory")

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

install(FILES openni2_redist/${HOST_PLATFORM}/libOpenNI2.so
  DESTINATION lib/
  )

install(DIRECTORY openni2_redist/${HOST_PLATFORM}/OpenNI2
  DESTINATION lib/
  )

install(TARGETS list_devices_node
  DESTINATION lib/${PROJECT_NAME}/
  )

install(
  DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
  )

install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}/
  )


install(DIRECTORY
  params
  DESTINATION share/${PROJECT_NAME}/
  )


install(DIRECTORY
  rviz
  DESTINATION share/${PROJECT_NAME}/
  )

if (BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif ()

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(${dependencies})
ament_package()
