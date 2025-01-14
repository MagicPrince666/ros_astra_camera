/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2013-2022 Orbbec 3D Technology, Inc                      */
/*                                                                        */
/* PROPRIETARY RIGHTS of Orbbec 3D Technology are involved in the         */
/* subject matter of this material. All manufacturing, reproduction, use, */
/* and sales rights pertaining to this subject matter are governed by the */
/* license agreement. The recipient of this software implicitly accepts   */
/* the terms of the license.                                              */
/*                                                                        */
/**************************************************************************/

#pragma once
#include <rclcpp/rclcpp.hpp>

namespace astra_camera {
class ParametersBackend {
 public:
  explicit ParametersBackend(rclcpp::Node* node);
  ~ParametersBackend();
  void addOnSetParametersCallback(
#if defined(USE_IRON_VERSION)
      rclcpp::node_interfaces::NodeParametersInterface::OnSetParametersCallbackType callback
#else
      rclcpp::node_interfaces::NodeParametersInterface::OnParametersSetCallbackType callback
#endif
  );

 private:
  rclcpp::Node* node_;
  rclcpp::Logger logger_;
#if defined(USE_ELOQUENT_VERSION) || defined(USE_DASHING_VERSION)
  rclcpp::Node::OnParametersSetCallbackType ros_callback_;
#else
  std::shared_ptr<void> ros_callback_;
#endif
};
}  // namespace astra_camera
