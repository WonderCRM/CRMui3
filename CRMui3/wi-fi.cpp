//http://developer.alexanderklimov.ru/arduino/esp32/wifi.php
#include "CRMui3.h"


void CRMui3::wifiEvent() {
  static bool firstConnection = false;

#ifdef ESP32
  WiFi.onEvent([this](system_event_id_t event, system_event_info_t info) {
    const int r = info.disconnected.reason;
    //Serial.println("[WiFi] Event: " + String(event) + ",  Reason: " + String(r));

    switch (event) {
      /*case 4: // STA_CONNECTED
        break;*/

      case 5: // STA_DISCONNECTED
        if (r == 2 || r == 7 || r == 15) WiFi.reconnect();
        else if (!firstConnection && (_wifiMode == 1 || _wifiMode == 3) &&
						millis() - _connectingTimer >= _waitTimeForConnection) {
          WiFi.disconnect();
          SPLN(String() + F("[WiFi] Connecting to WiFi failed. Time is over. Code ") + String(r));
        }
        break;

      case 7: // STA_GOT_IP
        SPLN(String() + F("[WiFi] Connecting to \"") + WiFi.SSID() + F("\" is done. ") +
             F("(IP: ") + WiFi.localIP().toString() + F(")"));
        if (!firstConnection && r != 200 && _wifiMode == 1) {
		  firstConnection = true;
          WiFi.mode(WIFI_STA);
          SPLN(F("[WiFi] AP mode disable."));
        }
        break;
    }
  });

#else

  // ESP8266
  static WiFiEventHandler Disconnected, GotIP;

  //wifi evt: 0
  /*Connected = WiFi.onStationModeConnected([this](WiFiEventStationModeConnected event) {
    });*/

  //wifi evt: 1
  Disconnected = WiFi.onStationModeDisconnected([this](WiFiEventStationModeDisconnected event) {
    //Serial.printf("*Disconnected from SSID: %s\n", event.ssid.c_str());
    //Serial.printf("*Reason: %d\n", event.reason);
    if ((_wifiMode == 1 || _wifiMode == 3) && !firstConnection &&
        millis() - _connectingTimer >= _waitTimeForConnection) {
      WiFi.disconnect();
      SPLN(String(F("[WiFi] Connecting to WiFi failed. Time is over. Code ")) + String(event.reason));
    }
  });

  //wifi evt: 3
  GotIP = WiFi.onStationModeGotIP([this](WiFiEventStationModeGotIP event) {
    firstConnection = true;
    SPLN(String(F("[WiFi] Connecting to \"")) + WiFi.SSID() + F("\" is done. ") +
         F("IP: ") + WiFi.localIP().toString());  //event.ip.toString().c_str()   event.ssid.c_str()
    if (_wifiMode == 1) {
      WiFi.mode(WIFI_STA);
      SPLN(F("[WiFi] AP mode disable."));
    }
  });
#endif
}


void CRMui3::wifiSTA() {
  WiFi.begin(var(F("_s")).c_str(), var(F("_p")).c_str());
}


void CRMui3::wifiAP() {
  WiFi.softAP(var(F("_as")).c_str(), var(F("_ap")).c_str());
  SPLN(String(F("[WiFi] AP name: ")) + var(F("_as")) +
       F("\t(IP: ") + WiFi.softAPIP().toString() + F(")"));
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
  _connectingTimer = millis();
}



/*void CRMui3::scan(bool startScan) {
  return;

    static uint32_t timer;
    if (timer + 1000 > millis()) return;
    timer = millis();
    // if(WiFi.scanComplete() != )
    networks = WiFi.scanNetworks();

    int n = WiFi.scanComplete();
    if (n >= 0) {
    Serial.printf("%d network(s) found\n", n);
    for (int i = 0; i < n; i++)
    {
      // Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    }
    // WiFi.scanDelete();
    }
  }*/

/*----------- ERROR CODE -----------
   esp_wifi_types.h, esp_event_legacy.h

   Reason
   200  BEACON_TIMEOUT
   201  AP not found
   202  AUTH_FAIL
   203  ASSOC_FAIL
   204  HANDSHAKE_TIMEOUT
   205  CONNECTION_FAIL
   206  AP_TSF_RESET
*/
