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
#include "web/icon-192x192.png.h"
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


void CRMui3::begin(const String &app_name, void (*uiFunction)(), void (*updateFunction)(), void (*apiFunction)(const String), uint32_t baud) {
  if (baud > 0) {
    Serial.begin(baud);
    Serial.setTimeout(100);
    Serial.flush();
    Serial.println();
    _debug = true;
  }
  _app_name = app_name;
	Serial.println(_app_name);
  {
    String m(F("\nCRMui3 ver:"));
    m += CRM_VER;
    m += F("\nDevice ID: ");
    m += getID();
    m += F("\n");
    SPLN(m);
  }
  if (uiFunction == NULL) {
    SPLN(F("[ERROR] uiFunction not defined"));
    return;
  }
  ui = uiFunction;
  if (updateFunction != NULL) upd = updateFunction;
  if (apiFunction != NULL) api = apiFunction;
  cfgLoad();
  if (_disableWiFiManagement) {
    WiFi.mode(WIFI_AP);
  } else wifiStart();
  http();
  server.begin();
  if (upd) {
    upd();
    ui();
  }
  if (_useArduinoOta) {
    ArduinoOTA.setHostname(var(F("_as")).c_str());
    ArduinoOTA.begin();
  }
  if (WiFi.getMode() > 1) dnsServer.start(53, "*", WiFi.softAPIP());
  _start = false;
}


void CRMui3::run(bool wmt) {
  if (_useArduinoOta) ArduinoOTA.handle();
  if (wmt || millis() - _runTimer >= 1000) {
    _runTimer = millis();
    _upTime++;
    if (WiFi.getMode() > 1) dnsServer.processNextRequest();
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
  if (ver != "") {
    if (!_project_version) _project_version = new String;
    *_project_version = ver;
  } else {
    if (_project_version) {
      delete _project_version;
      _project_version = nullptr;
    }
  }
}


uint32_t CRMui3::upTimeSec() {
	return _upTime;
}


String CRMui3::upTime(bool s) {
  if (s) return String(_upTime);
  else {
    static bool timeAdjustment = true;
    String b = String();
    if (timeAdjustment) {
      _upTime += (millis() / 1000 - _upTime);
      timeAdjustment = false;
    }
    if ((_upTime / 86400 % 365) != 0) {
      b += _upTime / 86400 % 365;
      b += F(",  ");
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
}


String CRMui3::getID() {
#ifdef ESP32
  return uint64ToStr(ESP.getEfuseMac());
#else
  return String(ESP.getChipId()) + String(ESP.getFlashChipId());
#endif
}


void CRMui3::defaultWifi(uint8_t mode, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc) {
  if (var(F("_wm")) == F("null")) var(F("_wm"), (((mode == 1 || mode == 3) && ssid == "") ? 2 : mode));
  if (var(F("_as")) == F("null")) var(F("_as"), (ap_ssid == "" ? String(F("CRMui3-")) + getID() : ap_ssid));
  if (var(F("_ap")) == F("null")) var(F("_ap"), ap_pass);
  if (var(F("_s")) == F("null")) var(F("_s"), ssid);
  if (var(F("_p")) == F("null")) var(F("_p"), pass);
  if (var(F("_wt")) == F("null")) var(F("_wt"), wtc);
}


// https://github.com/me-no-dev/ESPAsyncWebServer#async-websocket-event
// https://randomnerdtutorials.com/esp32-web-server-sent-events-sse/
void CRMui3::http() {
  ws.onEvent([this](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    ws.cleanupClients();
    if (type == WS_EVT_CONNECT) {
      _sendingToWeb = true;
      //client->ping();   //на iOS падает WebSocket
      String m = String();
      m += (F("[WS] ID "));
      m += client->id();
      m += F(" Connect");
      SPLN(m);
    } else if (type == WS_EVT_DISCONNECT) {
      String m = String();
      m += (F("[WS] ID "));
      m += client->id();
      m += F(" Disconnect");
      SPLN(m);
      if (ws.count() < 1) {
        _sendingToWeb = false;
        if (_bufWebUpdate) {
          delete _bufWebUpdate;
          _bufWebUpdate = nullptr;
        }
        if (_bufUI) {
          delete _bufUI;
          _bufUI = nullptr;
        }
      }
    } else if (type == WS_EVT_ERROR) {
      String m = String();
      m += (F("[WS] ID "));
      m += client->id();
      m += F(" Error");
      SPLN(m);
      if (ws.count() < 1) _sendingToWeb = false;
    }/* else if (type == WS_EVT_PONG) {
      Serial.printf("[WebSocket] ID %u. Pong %u: %s\n", client->id(), len, (len) ? (char*)data : "");
    } else if (type == WS_EVT_DATA) {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len) {
        if (info->opcode == WS_TEXT) { // если текстовые данные вебсокета
          data[len] = 0;
          //SPLN(String(F("[WS] ID ")) + String(client->id()) + F(" Recived: ") + String((char*)data));
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
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    _bufUI = new String;
    *_bufUI += F("{\"_t\":0,\"an\":\"");
    *_bufUI += _app_name;
    *_bufUI += F("\",\"id\":\"");
    *_bufUI += getID();
    *_bufUI += F("\",\"fw\":\"");
    *_bufUI += (_project_version ? *_project_version : CRM_VER);
    *_bufUI += F("\",\"a\":");
    *_bufUI += (_WebAuthLogin ? true : false);
    if (_eMail) {
      *_bufUI += F(",\"em\":\"");
      *_bufUI += *_eMail;
      *_bufUI += F("\"");
    }
    if (_telega) {
      *_bufUI += F(",\"tg\":\"");
      *_bufUI += *_telega;
      *_bufUI += F("\"");
    }
    if (_homePage) {
      *_bufUI += F(",\"hp\":\"");
      *_bufUI += *_homePage;
      *_bufUI += F("\"");
    }
    *_bufUI += F(",\"c\":[");
    ui();
    *_bufUI += F("]],\"cfg\":");
    serializeJson(cfg, *_bufUI);
    *_bufUI += F("}");
    /*if (b.length() > 8192) request->send_P(200, F("application/json"), b.c_str());
      else request->send(200, F("application/json"), b);*/
    request->send(200, F("application/json"), (*_bufUI).c_str());
    delete _bufUI;
    _bufUI = nullptr;
  });


  server.on("/set", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    request->send(200);
    uint8_t headers = request->params();
    for (uint8_t i = 0; i < headers; i++) {
      AsyncWebParameter* p = request->getParam(i);
      String pname(p->name());
      if (pname.indexOf("BT_") != -1) {
        if (!_bufBtnui) _bufBtnui = new String;
        *_bufBtnui = pname.substring(3);
        if (btnCallbackFunc) btnCallbackFunc((*_bufBtnui).c_str());
      } else if (pname == "wUPD") {
        String m(F("{\"_t\":0}"));
        ws.textAll(m.c_str());
      }
      else {
        String l = String();;
        l += (F("[VAR] "));
        l += pname;
        l += F(" = ");
        l += p->value();
        SPLN(l);
        if (pname.indexOf("CR_") != -1) {
          pname = pname.substring(3);
          var(pname, p->value(), false);
        } else var(pname, p->value());
        if (webConnCountStatus() > 1) {
          String m = String();
          m += (F("{\"_t\":2,\"d\":[[\""));
          m += pname;
          m += F("\",\"");
          m += p->value();
          m += F("\"]]}");
          ws.textAll(m.c_str());
        }
        if (upd) upd();
      }
    }
  });


  if (api) {
    server.on("/api", HTTP_GET, [this](AsyncWebServerRequest * request) {
      if (_WebAuthLogin) {
        if (!_apiKey) {
          if (!request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
            return request->requestAuthentication();
        } else {
          if (request->getParam(0)->value() != (*_apiKey))
            return request->requestAuthentication();
        }
      }
      uint32_t t = micros();
      String s = String();
      s += (F("{"));
      for (uint8_t i = 0; i < request->params(); i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (i) s += F(",");
        s += F("\"");
        s += p->name();
        s += F("\":\"");
        s += p->value();
        s += F("\"");
      }
      s += F("}");
      if (!_bufApiResponse) _bufApiResponse = new String;
      api(s);
      String resp = String();
      resp += (F("{\"requestLeadTime_ms\":"));
      resp += String(((micros() - t) * 0.001), 3);
      resp += *_bufApiResponse;
      if (_bufApiResponse) {
        delete _bufApiResponse;
        _bufApiResponse = nullptr;
      }
      resp += F("}");
      request->send(200, F("application/json"), resp);
    });
  }


  server.on("/reset", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    String s(F("ESP Reset. Device reboot..."));
    s += F("\nPlease connect to Access point: ");
    s += var(F("_as"));
    request->send(200, F("text/plain"), s);
    request->client()->close();
    cfgDelete();
  });


  server.on("/cfgdownload", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    cfgSave();
    if (SPIFFS.exists(F("/config.json"))) {
      File cf = SPIFFS.open(F("/config.json"), "r");
      String s(cf.readString());
      cf.close();
      AsyncWebServerResponse *response = request->beginResponse(200, F("application/json"), s);
			String h(F("attachment; filename=\""));
			h += _app_name;
			h += F("_config.json\"");
			response->addHeader(F("Content-Disposition"), h);
      request->send(response);
    } else {
      request->send(404, F("text/html"), F("Configuration file not found"));
    }
  });


  server.on("/cfgupload", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), F("ERROR, go back and try again"));
    response->addHeader(F("Connection"), F("close"));
    request->send(response);
  }, [this](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();

    if (request->header(F("Content-Length")).toInt() > 6000) {
      request->send(413, F("text/html"), F("Aborted upload because file size too big"));
      request->client()->close();
      String m(F("File size "));
      m += filename;
      m += F(" too big.");
      SPLN(m);
      return;
    }

    if (!_bufUld) _bufUld = new String;
    for (size_t i = 0; i < len; i++) {
      *_bufUld += (char)data[i];
    }

    if (final) {
      String m(F("File "));
      m += filename;
      m += F(" upload.");
      SPLN(m);
      if ((*_bufUld).startsWith(F("{")) && (*_bufUld).endsWith(F("}"))) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, (*_bufUld));
        if (_bufUld) {
          delete _bufUld;
          _bufUld = nullptr;
        }
        for (JsonPair kv : doc.as<JsonObject>()) {
          String n(kv.key().c_str());
          var(n, kv.value().as<String>());
        }
        if (upd) upd();
        request->send(200, F("text/html"), F("Config file update"));
        m = F("Configuratoin from ");
        m += filename;
        m += F(" write to SPIFFS");
        SPLN(m);
      } else {
        if (_bufUld) {
          delete _bufUld;
          _bufUld = nullptr;
        }
        request->send(200, F("text/html"), F("Configuration file content does not meet requirements"));
        m = F("Content into ");
        m += filename;
        m += F(" file, does not meet requirements");
        SPLN(m);
      }
    }
  });


  server.on("/update", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), F("ERROR, go back and try again"));
    response->addHeader(F("Connection"), F("close"));
    request->send(response);
  }, [this](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      String m(F("Update started, please wait.\nFirmware file: "));
      m += filename;
      SPLN(m);
#ifndef ESP32
      Update.runAsync(true);
#endif
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) Update.printError(Serial);
    }
    if (!Update.hasError() && Update.write(data, len) != len) Update.printError(Serial);
    if (final) {
      if (Update.end(true)) {
        SPLN(F("Update success"));
        _espNeedReboot = !Update.hasError();
        request->send(200, F("text/html"), (_espNeedReboot ? F("UPDATE SUCCESS!") : F("UPDATE FAILED!")));
      } else {
        request->send(200, F("text/html"), F("UPDATE ERROR! Please repeat"));
        Update.printError(Serial);
      }
    }
  });


  server.on("/wifi", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    auto s = wifiScan();
    SPLN(s);
    request->send(200, F("application/json"), s);
  });


  // ********** Page files **********
  server.on("/", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
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
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
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
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
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
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), notif_js, notif_js_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });


  server.on("/notif.css", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/css"), notif_css, notif_css_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });


  server.on("/chart.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    if (_useChart) {
      AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), chart_js, chart_js_size);
      response->addHeader(F("Content-Encoding"), F("gzip"));
      request->send(response);
    } else request->send(200);
  });


  server.on("/gauge.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    if (_useGauge) {
      AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), gauge_js, gauge_js_size);
      response->addHeader(F("Content-Encoding"), F("gzip"));
      request->send(response);
    } else request->send(200);
  });


  server.on("/fonts/font.woff2", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("font/woff2"), font_woff2, font_woff2_size);
    request->send(response);
  });


  server.on("/fonts/fonticon.woff2", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("font/woff2"), fonticon_woff2, fonticon_woff2_size);
    request->send(response);
  });


  server.on("/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/x-icon"), favicon_ico, favicon_ico_size);
    request->send(response);
  });


  server.on("/icon.png", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), icon_192x192, icon_192x192_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });


  server.on("/manifest.json", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    String m(F("{\"background_color\":\"#333\",\"display\":\"standalone\",\"start_url\":\"/\",\"name\":\""));
    m += _app_name;
    m += F("\",\"short_name\":\"");
    m += _app_name;
    m += F("\",\"icons\":[{\"src\":\"/icon.png\",\"sizes\":\"192x192\",\"type\":\"image/png\"}]}");
    request->send(200, F("application/json"), m);
  });


  server.on("/logout", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_WebAuthLogin && !request->authenticate((*_WebAuthLogin).c_str(), (*_WebAuthPass).c_str()))
      return request->requestAuthentication();
    request->send(401);
  });


  server.onNotFound([this](AsyncWebServerRequest * request) {
    //request->send(404, F("text / html"), F("Page not found"));
    request->redirect("/");
  });
}


void CRMui3::contacts(const String &e, const String &t, const String &h) {
  if (!_eMail && e != "") {
    _eMail = new String;
    *_eMail = e;
  }
  if (!_telega && t != "") {
    _telega = new String;
    *_telega = t;
  }
  if (!_homePage && h != "") {
    _homePage = new String;
    String _h(h);
    _h.replace(F("\\"), F(" / "));
    *_homePage = _h;
  }
}


void CRMui3::setWebAuth(const String &login, const String &pass) {
  if (login != "") {
    if (!_WebAuthLogin) _WebAuthLogin = new String;
    *_WebAuthLogin = login;
    if (!_WebAuthPass) _WebAuthPass = new String;
    *_WebAuthPass = pass;
  } else {
    if (_WebAuthLogin) {
      delete _WebAuthLogin;
      _WebAuthLogin = nullptr;
    }
    if (_WebAuthPass) {
      delete _WebAuthPass;
      _WebAuthPass = nullptr;
    }
  }
}


void CRMui3::setApiKey(const String &key) {
  if (api) {
    if (key == "") {
      if (_apiKey) {
        delete _apiKey;
        _apiKey = nullptr;
      }
    } else {
      if (!_apiKey) _apiKey = new String;
      *_apiKey = key;
    }
  }
}


uint8_t CRMui3::webConnCountStatus() {
  return ws.count();
}


bool CRMui3::webConnStatus() {
  return _sendingToWeb;
}


bool CRMui3::apiResponse(const String & p, const String & v) {
  if (!_bufApiResponse) return false;
  String b = String();
  b += *_bufApiResponse;
  b += F(",\"");
  b += p;
  b += F("\":\"");
  b += v;
  b += F("\"");
  *_bufApiResponse = b;
}


void CRMui3::webUpdate(const String & name, const String & value, bool n) {
  if (_sendingToWeb) {
    if (name == "") {
      String m(F("{\"_t\":0}"));
      ws.textAll(m.c_str());
    } else {
      String b = String();
      if (_bufWebUpdate) {
        b = *_bufWebUpdate;
        b += F(",");
      } else {
        _bufWebUpdate = new String;
        b = F("{\"_t\":2,\"d\":[");
      }
      b += F("[\"");
      b += name;
      b += F("\",\"");
      b += value;
      b += F("\"]");
      uint16_t len = b.length();
      if (n || len > 350) {
        b += F("]}");
        ws.textAll(b.c_str());
        if (_bufWebUpdate) {
          delete _bufWebUpdate;
          _bufWebUpdate = nullptr;
        }
      } else *_bufWebUpdate = b;
    }
  }
}


void CRMui3::webNotif(const String & type, const String & text, long tout, bool x) {
  if (_sendingToWeb) {
    String m(F("{\"_t\":3,\"d\":[\""));
    m += type;
    m += "\",\"";
    m += text;
    m += "\",";
    m += tout;
    m += ",";
    m += x;
    m += "]}";
    ws.textAll(m.c_str());
  }
}


String CRMui3::getLang() {
  return var(F("_L"));
}


void CRMui3::espReboot() {
  cfgSave();
  delay(200);
  SPLN(F("ESP Reboot..."));
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
  S=nullptr;
  }*/


void CRMui3::espSleep(uint32_t sec, bool m) {
#ifdef ESP32
  if (sec) {
		SP(F("[SLEEP] Sleep timer on "));
		SP(sec);
		SPLN(F(" second started..."));
		esp_sleep_enable_timer_wakeup(sec * 1000000ULL);
	} else if (m) {
    SPLN(F("[SLEEP] Deep sleep started..."));
    esp_deep_sleep_start();
  } else {
    SPLN(F("[SLEEP] Light sleep started..."));
    esp_light_sleep_start();
  }
  SPLN(F("[SLEEP] ESP woke up"));
  _upTime += sec;
  //esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
#else
  Serial.println(F("The code for ESP8266 not written.\nPlease view exemple esp8266/LowPowerDemo/LowPowerDemo.ino"));
#endif
}
