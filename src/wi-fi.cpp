//http://developer.alexanderklimov.ru/arduino/esp32/wifi.php
#include "CRMui3.h"


void CRMui3::wifiEvent() {
  static bool firstConnection = false;

#ifdef ESP32
  WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
    //Serial.println("[WiFi] Event: " + String(event) + ",  Reason: " + String(r));

    // Core 1.0.6    v3.3.5-1-g85c43024c
    const int r = info.disconnected.reason;

    // Core 2.0.0+   v4.4-dev-2313-gc69f0ec32
    //const int r = info.wifi_sta_disconnected.reason;

    switch (event) {
      /*case 4: // STA_CONNECTED
        break;*/

      case 5: // STA_DISCONNECTED
        if (r == 2 || r == 4 || r == 7 || r == 8 || r == 15) WiFi.reconnect();
        else if (!firstConnection && (_wifiMode == 1 || _wifiMode == 3) && _waitTimeForConnection != 0 &&
                 millis() - _connectingTimer >= _waitTimeForConnection) {
          WiFi.disconnect();
          {
            String m = String();
            m += F("[WiFi] Connecting to WiFi failed. Time is over. Code ");
            m += r;
            SPLN(m);
          }
        }
        break;

      case 7: // STA_GOT_IP
        {
          String m = String();
          m += F("[WiFi] Connecting to \"");
          m += WiFi.SSID();
          m += F("\" is done. ");
          m += F("(IP: ");
          m += WiFi.localIP().toString();
          m += F(")");
          SPLN(m);
        }
        if (!firstConnection && r != 200) {
          firstConnection = true;
          if (_wifiMode == 1) {
            WiFi.mode(WIFI_STA);
            SPLN(F("[WiFi] AP mode disable"));
          }
        }
        break;

      /*case 17: // SYSTEM_EVENT_AP_STADISCONNECTED
        break;*/

      default:
        break;
    }
  });

#else

  // ESP8266
  static WiFiEventHandler Disconnected, GotIP;//, APDisconnected;

  //wifi evt: 0
  /*Connected = WiFi.onStationModeConnected([this](WiFiEventStationModeConnected event) {
    });*/

  //wifi evt: 1
  Disconnected = WiFi.onStationModeDisconnected([this](WiFiEventStationModeDisconnected event) {
    //Serial.printf("[WiFi][E%d][R%d] Disconnected from SSID: %s\n", event, event.reason, event.ssid.c_str());
    if (!firstConnection && (_wifiMode == 1 || _wifiMode == 3) && _waitTimeForConnection != 0 &&
        millis() - _connectingTimer >= _waitTimeForConnection) {
      WiFi.disconnect();
      {
        String m = String();
        m += F("[WiFi] Connecting to WiFi failed. Time is over. Code ");
        m += event.reason;
        SPLN(m);
      }
    }
  });

  //wifi evt: 3
  GotIP = WiFi.onStationModeGotIP([this](WiFiEventStationModeGotIP event) {
    {
      String m = String();
      m += F("[WiFi] Connecting to \"");
      m += WiFi.SSID();
      m += F("\" is done. ");
      m += F("IP: ");
      m += WiFi.localIP().toString();
      //event.ip.toString().c_str()   event.ssid.c_str()
      SPLN(m);
    }
    if (!firstConnection) {
      firstConnection = true;
      if (_wifiMode == 1) {
        WiFi.mode(WIFI_STA);
        SPLN(F("[WiFi] AP mode disable"));
      }
    }
  });

  //wifi evt: 6
  /*APDisconnected = WiFi.onSoftAPModeStationDisconnected([this](WiFiEventSoftAPModeStationDisconnected event) {
    });*/

#endif
}


void CRMui3::wifiSTA() {
  WiFi.begin(var(F("_s")).c_str(), var(F("_p")).c_str());
}


void CRMui3::wifiAP() {
  WiFi.softAP(var(F("_as")).c_str(), var(F("_ap")).c_str());
  String m = String();
  m += F("[WiFi] AP name: ");
  m += var(F("_as"));
  m += F("\t(IP: ");
  m += WiFi.softAPIP().toString();
  m += F(")");
  SPLN(m);
}


void CRMui3::wifiStart() {
  _wifiMode = var(F("_wm")).toInt();
  _waitTimeForConnection = var(F("_wt")).toInt() * 1000UL;
  WiFi.persistent(false);

  wifiEvent();
  wifiAP();
  if (_wifiMode == 1) wifiSTA();
  else if (_wifiMode == 3) {
    WiFi.mode(WIFI_AP_STA);
    wifiSTA();
  }
  if (_wifiMode == 2) WiFi.scanNetworks(true);
  _connectingTimer = millis();
}


String CRMui3::wifiScan() {
  int n = WiFi.scanComplete();
  if (n > 0) {
    String s(F("{\"s\":"));
    s += n;
    s += F(",\"n\":[");
    for (int i = 0; i < n; ++i) {
      if (i)s += F(",");
      s += F("[\"");
      s += WiFi.SSID(i);
      s += F("\",");
      s += WiFi.channel(i);
      s += F(",");
      s += WiFi.RSSI(i);
      s += F(",");
      s += WiFi.encryptionType(i);
      s += F("]");
    }
    s += F("]}");
    WiFi.scanNetworks(true);
    return s;
  } else if (n == 0) {
    return F("{\"s\":0}");
  } else if (n == -1) {
    return F("{\"s\":-1}");
  } else if (n == -2) {
    WiFi.scanNetworks(true);
    return F("{\"s\":-2}");
  } else return F("{\"s\":0}");
}

/*----------- ERROR CODE -----------
   ESP8266  ESP8266WiFiType.h
   ESP32    esp_wifi_types.h, esp_event_legacy.h

   [WiFi][R...]Reason
   200  BEACON_TIMEOUT
   201  AP not found
   202  AUTH_FAIL
   203  ASSOC_FAIL
   204  HANDSHAKE_TIMEOUT
   205  CONNECTION_FAIL
   206  AP_TSF_RESET
*/

/*------ WiFi Encryption Type ------
  0   WIFI_AUTH_OPEN
  1   WIFI_AUTH_WEP
  2   WIFI_AUTH_WPA_PSK
  3   WIFI_AUTH_WPA2_PSK
  4   WIFI_AUTH_WPA_WPA2_PSK
  5   WIFI_AUTH_WPA2_ENTERPRISE
  6   WIFI_AUTH_MAX
*/
