#define _gz

#include "CRMui3.h"

#ifdef _gz
#include "web/index.html.h"
#include "web/script.js.h"
#include "web/style.css.h"
#else
#include "web_index.html.h"
#include "web_script.js.h"
#include "web_style.css.h"
#endif
#include "web/notif.js.h"
#include "web/notif.css.h"
#include "web/chart.js.h"
#include "web/gauge.js.h"
#include "web/font.woff2.h"
#include "web/fonticon.woff2.h"
#include "web/favicon.ico.h"


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;


void CRMui3::disableWiFiManagement() {
  _disableWiFiManagement = true;
}


void CRMui3::useArduinoOta() {
  _useArduinoOta = true;
}


void CRMui3::begin(const String &app_name, void (*uiFunction)(), void (*updateFunction)(), void (*apiFunction)(String), uint32_t baud) {
  if (baud > 0) {
    Serial.begin(baud);
    Serial.setTimeout(100);
    Serial.flush();
    _debug = true;
  }
  SPLN(String(F("\nCRMui3 ver:")) + CRM_VER);
  if (uiFunction == NULL) {
    SPLN(F("[ERROR] uiFunction not defined."));
    return;
  }
  ui = uiFunction;
  if (updateFunction != NULL) upd = updateFunction;
  if (apiFunction != NULL) api = apiFunction;
  _app_name = app_name;
  getID();
  cfgLoad();
  if (_disableWiFiManagement) WiFi.mode(WIFI_AP);
  else wifiStart();
  http();
  server.begin();
  if (upd) upd();
  if (_useArduinoOta) {
    ArduinoOTA.setHostname(var(F("_as")).c_str());
    ArduinoOTA.begin();
  }
  if (_disableWiFiManagement) {
    _wifiMode = WiFi.getMode();
    if (_wifiMode > 1) dnsServer.start(53, "*", WiFi.softAPIP());
  }
  _start = false;
}


void CRMui3::run() {
  if (_useArduinoOta) ArduinoOTA.handle();
  if (millis() - _runTimer >= 1000) {
    _runTimer = millis();
    _upTime++;
    if (_wifiMode > 1) dnsServer.processNextRequest();
    cfgAutoSave();
    if (_sendingToWeb) {
      _sendingToWeb = webConnCountStatus();
      if (WiFi.getMode() == 2 && !WiFi.softAPgetStationNum()) _sendingToWeb = false;
      webUpdate("uptime", upTime());
      webUpdate("wifi", String(WiFi.RSSI()));
      webUpdate("ram", String(ESP.getFreeHeap()), true);
    }
    if (_espNeedReboot) espReboot();
  }
}


void CRMui3::version (const String &ver) {
  _project_version = ver;
}


String CRMui3::upTime() {
  static bool timeAdjustment = true;
  String b = String();
  if (timeAdjustment) {
    _upTime += millis() / 1000 - _upTime;
    timeAdjustment = false;
  }
  if ((_upTime / 86400 % 365) != 0) {
    b += _upTime / 86400 % 365;
    b += ",  ";
  }
  b += _upTime / 3600 % 24;
  b += ":";
  if ((_upTime / 60 % 60) < 10) b += "0";
  b += _upTime / 60 % 60;
  b += ":";
  if ((_upTime % 60) < 10) b += "0";
  b += _upTime % 60;
  return b;
}


void CRMui3::getID() {
#ifdef ESP32
  _id = uint64ToStr(ESP.getEfuseMac());
#else
  _id = String(ESP.getChipId()) + String(ESP.getFlashChipId());
#endif
  SPLN(String(F("Device ID: ")) + _id + F("\n"));
}


void CRMui3::defaultWifi(uint8_t mode, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc) {
  if (var(F("_wm")) == F("null")) var(F("_wm"), (((mode == 1 || mode == 3) && ssid == "") ? 2 : mode));
  if (var(F("_as")) == F("null")) var(F("_as"), (ap_ssid == "" ? String(F("CRMui3-")) + _id : ap_ssid));
  if (var(F("_ap")) == F("null")) var(F("_ap"), ap_pass);
  if (var(F("_s")) == F("null")) var(F("_s"), ssid);
  if (var(F("_p")) == F("null")) var(F("_p"), pass);
  if (var(F("_wt")) == F("null")) var(F("_wt"), wtc);
}


// https://github.com/me-no-dev/ESPAsyncWebServer#async-websocket-event
// https://randomnerdtutorials.com/esp32-web-server-sent-events-sse/
void CRMui3::http() {
  ws.onEvent([this](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      _sendingToWeb = true;
      ws.cleanupClients();
      //client->ping();
      DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Connect"));
    } else if (type == WS_EVT_DISCONNECT) {
      DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Disconnect"));
      if (ws.count() < 1) _sendingToWeb = false;
    } else if (type == WS_EVT_ERROR) {
      DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Error"));
      if (ws.count() < 1) _sendingToWeb = false;
    }/* else if (type == WS_EVT_PONG) {
      Serial.printf("[WebSocket] ID %u. Pong %u: %s\n", client->id(), len, (len) ? (char*)data : "");
    } else if (type == WS_EVT_DATA) {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len) {
        if (info->opcode == WS_TEXT) { // если текстовые данные вебсокета
          data[len] = 0;
          //DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Recived: ") + String((char*)data));
          if (String((char*)data) == "i") {    //deserializeJson(json, reinterpret_cast<const char*>(data));
  #ifdef ESP32
            delay(50);
  #endif
          }
        } else { // если бинарные данные вебсокета
          char buff[3];
          for (size_t i = 0; i < info->len; i++) {
            sprintf(buff, "%02x ", (uint8_t) data[i]);
            msg += buff ;
          }
          Serial.printf("%s\n", msg.c_str());
        }
      }
    }*/
  });

  server.addHandler(&ws);


  server.on("/ui", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    _buf = String(F("{\"_t\":0,\"an\":\"")) + _app_name + F("\",\"id\":\"") + _id + F("\",\"fw\":\"") +
           (_project_version == "" ? CRM_VER : _project_version) +
           F("\",\"a\":") + String(_AuthenticateStatus);
    _buf += String(F(",\"lic\":\"")) + _licKey + F("\"");
    if (_eMail != "") _buf += String(F(",\"em\":\"")) + _eMail + "\"";
    if (_telega != "") _buf += String(F(",\"tg\":\"")) + _telega + "\"";
    if (_homePage != "") _buf += String(F(",\"hp\":\"")) + _homePage + "\"";
    _buf += F(",\"c\":[");
    ui();
    _buf += F("],\"cfg\":");
    serializeJson(cfg, _buf);
    _buf += "}";
    if (_buf.length() > 7000) request->send_P(200, F("text/plain"), _buf.c_str());
    else request->send(200, F("text/plain"), _buf);
    _buf = String();
  });


  server.on("/set", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send(200);
    int headers = request->params();
    for (int i = 0; i < headers; i++) {
      AsyncWebParameter* p = request->getParam(i);
      String pname = p->name();
      if (pname.indexOf("BT_") != -1) {
        _btnui = pname.substring(3);
        if (btnCallbackFunc) btnCallbackFunc(_btnui.c_str());
      } else if (pname == "wUPD") ws.textAll(String("{\"_t\":0}").c_str());
      else {
        DBGLN("[VAR] " + pname + F(" = ") + p->value());
        if (pname.indexOf("CR_") != -1) {
          pname = pname.substring(3);
          var(pname, p->value(), false);
        } else var(pname, p->value());
        if (webConnCountStatus() > 1) ws.textAll(String("{\"_t\":2,\"d\":[[\"" + pname + "\",\"" + p->value() + "\"]]}").c_str());
        if (upd) upd();
      }
    }
  });


  if (api) {
    server.on("/api", HTTP_GET, [this](AsyncWebServerRequest * request) {
      if (_AuthenticateStatus) {
        if (_apiKey == "") {
          if (!request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
            return request->requestAuthentication();
        } else {
          if (request->getParam(0)->value() != _apiKey)
            return request->requestAuthentication();
        }
      }
      uint32_t t = micros();
      String s = "{";
      for (int i = 0; i < request->params(); i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (i) s += ",";
        s += "\"" + String(p->name()) + "\":\"" + String(p->value()) + "\"";
      }
      s += "}";
      _apiResponse = "{\"stateRequest\":\"OK\",\"leadTime_ms\":\"" + String(((float)(micros() - t) / 1000), 3) + "\"}";
      api(s);
      request->send(200, "text/plain", _apiResponse);
      _apiResponse = String();
    });
  }


  server.on("/reset", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    String s = F("ESP Reset. Device reboot...");
    s += F("\nPlease connect to Access point: ");
    s += var(F("_as"));
    request->send(200, F("text/plain"), s);
    request->client()->close();
    cfgDelete();
  });


  server.on("/cfgdownload", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    cfgSave();
    if (SPIFFS.exists(F("/config.json"))) {
      File cf = SPIFFS.open(F("/config.json"), "r");
      String s = cf.readString();
      cf.close();
      AsyncWebServerResponse *response = request->beginResponse(200, F("application/json"), s);
      response->addHeader(F("Content-Disposition"), F("attachment; filename=\"config.json\""));
      request->send(response);
    } else request->send(404, F("text/html"), F("Configuration file not found."));
  });


  server.on("/cfgupload", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), F("ERROR, go back and try again."));
    response->addHeader(F("Connection"), F("close"));
    request->send(response);
  }, [this](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();

    if (request->header(F("Content-Length")).toInt() > 5140) {
      request->send(413, F("text/html"), F("Aborted upload because file size too big."));
      request->client()->close();
      Serial.println(String(F("File size ")) + filename + F(" too big."));
      return;
    }

    static String s = String();
    for (size_t i = 0; i < len; i++) {
      s += (char)data[i];
    }

    if (final) {
      Serial.println(String(F("File ")) + filename + F(" upload."));
      if (s.startsWith(F("{")) && s.endsWith(F("}"))) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, s);
        s = String();
        for (JsonPair kv : doc.as<JsonObject>()) {
          var(String(kv.key().c_str()), kv.value().as<String>());
        }
        if (upd) upd();
        request->send(200, F("text/html"), F("Config file update."));
        Serial.println(String(F("Configuratoin from ")) + filename + F(" write to SPIFFS."));
      } else {
        s = String();
        request->send(200, F("text/html"), F("Configuration file content does not meet requirements."));
        Serial.println(String(F("Content into ")) + filename + F(" file, does not meet requirements."));
      }
    }
  });


  server.on("/update", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), F("ERROR, go back and try again."));
    response->addHeader(F("Connection"), F("close"));
    request->send(response);
  }, [this](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      SPLN(String(F("Update started, please wait.\nFirmware file: ")) + filename);
#ifndef ESP32
      Update.runAsync(true);
#endif
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) Update.printError(Serial);
    }
    if (!Update.hasError() && Update.write(data, len) != len) Update.printError(Serial);
    if (final) {
      if (Update.end(true)) {
        SPLN(F("Update success."));
        _espNeedReboot = !Update.hasError();
        request->send(200, F("text/html"), _espNeedReboot ? F("UPDATE SUCCESS!") : F("UPDATE FAILED!"));
      } else {
        request->send(200, F("text/html"), F("UPDATE ERROR! Please repeat."));
        Update.printError(Serial);
      }
    }
  });


  server.on("/wifi", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    auto s = wifiScan();
    DBGLN(s);
    request->send(200, "text/plain", s);
  });


  // ********** Page files **********
  server.on("/", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
#ifdef _gz
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/html"), index_html, index_html_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
#else
    request->send_P(200, F("text/html"), index_html);
#endif
  });

  server.on("/script.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
#ifdef _gz
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), script_js, script_js_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
#else
    request->send_P(200, F("application/javascript"), script_js);
#endif
  });

  server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
#ifdef _gz
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/css"), style_css, style_css_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
#else
    request->send_P(200, F("text/css"), style_css);
#endif
  });

  server.on("/notif.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), notif_js, notif_js_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });


  server.on("/notif.css", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/css"), notif_css, notif_css_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });


  server.on("/chart.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    if (_useChart) {
      AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), chart_js, chart_js_size);
      response->addHeader(F("Content-Encoding"), F("gzip"));
      request->send(response);
    } else request->send(200);
  });


  server.on("/gauge.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    if (_useGauge) {
      AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), gauge_js, gauge_js_size);
      response->addHeader(F("Content-Encoding"), F("gzip"));
      request->send(response);
    } else request->send(200);
  });


  server.on("/fonts/font.woff2", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("font/woff2"), font_woff2, font_woff2_size);
    request->send(response);
  });


  server.on("/fonts/fonticon.woff2", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("font/woff2"), fonticon_woff2, fonticon_woff2_size);
    request->send(response);
  });


  server.on("/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/x-icon"), favicon_ico, favicon_ico_size);
    request->send(response);
  });


  server.on("/logout", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send(401);
  });


  server.onNotFound([this](AsyncWebServerRequest * request) {
    //request->send(404, F("text/html"), F("Page not found"));
    request->redirect("/");
  });
}


void CRMui3::license(const String &lic, const String &e, const String &t, const String &h) {
  _licKey = lic;
  _eMail = e;
  _telega = t;
  _homePage = h;
  _homePage.replace("\\", "/");
}


void CRMui3::setWebAuth(const String &login, const String &pass) {
  if (login != "") {
    _WebAuthLogin = login;
    _WebAuthPass = pass;
    _AuthenticateStatus = true;
  } else _AuthenticateStatus = false;
}


void CRMui3::setApiKey(const String &key) {
  if (api) _apiKey = key;
}


uint8_t CRMui3::webConnCountStatus() {
  return ws.count();
}


void CRMui3::apiResponse(const String &p, const String &v) {
  _apiResponse[_apiResponse.length() - 1] = ',';
  _apiResponse += "\"" + p + "\":\"" + v + "\"}";
}


void CRMui3::webUpdate(const String &name, const String &value, bool n) {
  if (_sendingToWeb) {
    if (name == "") ws.textAll(String("{\"_t\":0}").c_str());
    else {
      static String b = String();
      if (!b.startsWith("{")) b = "{\"_t\":2,\"d\":[";
      b += "[\"" + name + "\",\"" + value + "\"],";
      if (n) {
        b[b.length() - 1] = ']';
        b += "}";
        ws.textAll(b.c_str());
        b = String();
      }
    }
  }
}


void CRMui3::webNotif(const String &type, const String &text, long tout, bool x) {
  if (_sendingToWeb) {
    ws.textAll(String("{\"_t\":3,\"d\":[\"" + type + "\",\"" + text + "\"," +
                      String(tout) + "," + String(x) + "]}").c_str());
  }
}


String CRMui3::getLang() {
  return var("_L");
}


void CRMui3::espReboot() {
  cfgSave();
  delay(200);
  DBGLN(F("ESP Reboot..."));
  ESP.restart();
}


String CRMui3::uint64ToStr(uint64_t v) {
  String s = String();
  do {
    char c = v % 10;
    v /= 10;
    if (c < 10) c += '0';
    else c += 'A' - 10;
    s = c + s;
  } while (v);
  return s;
}


/*char* CRMui3::strToChr(String s) {
  char* S = new char[s.length() + 1];
  strcpy(S, s.c_str());
  return S;
  //delete[] S;
}*/


void CRMui3::espSleep(uint32_t sec, bool m) {
#ifdef ESP32
  if (sec) esp_sleep_enable_timer_wakeup(sec * 1000000ULL);
  if (m) {
    SPLN(F("[SLEEP] Deep sleep started..."));
    esp_deep_sleep_start();
  } else {
    SPLN(F("[SLEEP] Light sleep started..."));
    esp_light_sleep_start();
  }
  SPLN(F("[SLEEP] Light sleep ended."));
  _upTime += sec;
  //esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
#else
  Serial.println(F("The code for ESP8266 not written.\nPlease view exemple esp8266/LowPowerDemo/LowPowerDemo.ino"));
#endif
}
