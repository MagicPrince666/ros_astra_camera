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

#include "astra_camera/ros_param_backend.h"
namespace astra_camera {
ParametersBackend::ParametersBackend(rclcpp::Node *node)
    : node_(node), logger_(node_->get_logger()) {}

ParametersBackend::~ParametersBackend() {
  if (ros_callback_) {
#if !defined(USE_ELOQUENT_VERSION) && !defined(USE_DASHING_VERSION)
    node_->remove_on_set_parameters_callback(
        (rclcpp::node_interfaces::OnSetParametersCallbackHandle *)(ros_callback_.get()));
    ros_callback_.reset();
#endif
  }
}

void ParametersBackend::addOnSetParametersCallback(
#if defined(USE_IRON_VERSION)
    rclcpp::node_interfaces::NodeParametersInterface::OnSetParametersCallbackType callback
#else
    rclcpp::node_interfaces::NodeParametersInterface::OnParametersSetCallbackType callback
#endif
    ) {
#if defined(USE_ELOQUENT_VERSION) || defined(USE_DASHING_VERSION)
  ros_callback_ = node_->set_on_parameters_set_callback(callback);
#else
  ros_callback_ = node_->add_on_set_parameters_callback(callback);
#endif
}

}  // namespace astra_camera
