#include "CRMui3.h"


bool CRMui3::btnSwStatus() {
  return _btnui != String();
}


void CRMui3::btnCallback(const String &name, buttonCallback response) {
  if (_btnui == name) {
    _btnui = String();
    response();
  }
}


void CRMui3::btnCallback(int pin, buttonCallback response, uint8_t lvl) {
  if (digitalRead(pin) == lvl) {
    static uint32_t t = 0;
    uint32_t m = millis();
    if (m - t > 50) {
      response();
    }
    t = m;
  }
}
