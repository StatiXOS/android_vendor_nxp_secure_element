/******************************************************************************
 *
 *  Copyright 2018 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#define LOG_TAG "nxpese@1.0-service"
#include <android/hardware/secure_element/1.0/ISecureElement.h>
#include <hidl/LegacySupport.h>
#include <log/log.h>
#include <vendor/nxp/nxpese/1.0/INxpEse.h>

#include "NxpEse.h"
#include "SecureElement.h"
#include "StateMachine.h"
#include "ese_config.h"
#include "eSEClient.h"

// Generated HIDL files
using android::hardware::secure_element::V1_0::ISecureElement;
using android::hardware::secure_element::V1_0::implementation::SecureElement;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::OK;
using android::sp;
using android::status_t;
using vendor::nxp::nxpese::V1_0::INxpEse;
using vendor::nxp::nxpese::V1_0::implementation::NxpEse;

int main() {
  ALOGD("Initializing State Machine...");
  StateMachine::GetInstance().ProcessExtEvent(EVT_SPI_HW_SERVICE_START);

  ALOGD("Registering SecureElement HALIMPL Service v1.0...");
  sp<ISecureElement> se_service = new SecureElement();
  configureRpcThreadpool(2, true /*callerWillJoin*/);
  checkEseClientUpdate();
  std::string spiTermName;
  spiTermName = EseConfig::getString(NAME_NXP_SPI_TERMINAL_NAME, "eSE1");
  ALOGD("Registering SPI interface as %s", spiTermName.c_str());
  status_t status = se_service->registerAsService(spiTermName.c_str());
  if (status != OK) {
    LOG_ALWAYS_FATAL(
        "Could not register service for Secure Element HAL Iface (%d).",
        status);
    return -1;
  }

  ALOGD("Registering SecureElement HALIOCTL Service v1.0...");
  sp<INxpEse> nxp_se_service = new NxpEse();
  status = nxp_se_service->registerAsService();
  if (status != OK) {
    LOG_ALWAYS_FATAL(
        "Could not register service for Power Secure Element Extn Iface (%d).",
        status);
    return -1;
  }
  ALOGD("Secure Element HAL Service is ready");
  perform_eSEClientUpdate();
  joinRpcThreadpool();
  return 1;
}
