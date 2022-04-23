#include "camera_base.h"

void CameraBase::setEnableOnReady(bool value) {
    enable_on_ready = value;
}
bool CameraBase::getEnableOnReady() {
    return enable_on_ready;
}
