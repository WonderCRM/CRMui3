#ifndef CRMui3_h
#define CRMui3_h

#ifndef CRM_CONFIG_SIZE
	#define CRM_CONFIG_SIZE 4096
#endif

#include "defines.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>

#ifdef ESP32
#include "SPIFFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif


typedef struct {
  const String &label;
  const String &value;
} SelOpt;

typedef struct {
  const String &id;
  const String &label;
  const String &defaultValue;
  bool reload;
} Select;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String d;
  String s;
} Input;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String def;
  String c;
  String s;
} Output;

typedef struct {
  const String &id;
  const String &label;
  float defaultValue;
  float min;
  float max;
  float step;
  String unit;
} Range;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String icon;
  String color;
  bool newGroup;
} Card;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String labelLine;
  String valueLine;
  String color;
  String height;
} Chart;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  float minimum;
  float maximum;
  float def;
  String color[6][3];
  String unit;
  bool group;
} Gauge;



class CRMui3 {

    StaticJsonDocument<CRM_CONFIG_SIZE> cfg;

    typedef void (*buttonCallback)();
    typedef void (*buttonCallbackEvent)(const char *);
    typedef void (*uiCallback)();
    typedef void (*updateCallback)();
    typedef void (*apiCallback)(const String);

  public:
    CRMui3(): btnCallbackFunc(0) {}
    void begin(const String &app_name, void (*uiFunction)(), void (*updateFunction)() = NULL,
               void (*apiFunction)(const String) = NULL, uint32_t baud = 0);
    void disableWiFiManagement();
    void useArduinoOta();
    void run(bool wmt = false);
    String getID();
    String upTime(bool s = false);
		uint32_t upTimeSec();
    void cfgSave();
    void cfgDelete();
    void espReboot();
    String getLang();
    uint8_t webConnCountStatus();
    bool webConnStatus();
    String wifiScan();
    void espSleep(uint32_t sec = 0, bool m = false);
    void contacts(const String &e = "", const String &t = "", const String &h = "");
    void version (const String &ver);
    String uint64ToStr(uint64_t);
    //char* strToChr(String);

    void setWebAuth(const String &login, const String &pass = "");
    void setApiKey(const String &key);
    void page(const String &pageName);
    void group(uint8_t type);
    void selOpt(SelOpt);
    void select(Select);
    void input(Input);
    void output(Output);
    void range(Range);
    void card(Card);
    void chart(Chart);
    void gauge(Gauge);
    bool btnSwStatus();
    void btnCallback(const String &name, buttonCallback response);
    void btnCallback(uint8_t, buttonCallback response, uint8_t lvl = LOW);
    void btnCallback(buttonCallbackEvent func) {
      btnCallbackFunc = func;
    }
    void wifiForm(uint8_t, const String &ap_ssid = "", const String &ap_pass = "",
                  const String &ssid = "", const String &pass = "", long wtc = 120);
    void webUpdate(const String &name = "", const String &value = "", bool n = false);
    void webNotif(const String &type, const String &text, long tout = 5, bool x = true);
    bool apiResponse(const String &p, const String &v);

    String var(const String &name);
    void var(const String &name, const String &value, bool save = true);
    void var(const String &name, float, bool save = true);
    void var(const String &name, double, bool save = true);
    void var(const String &name, int, bool save = true);
    void var(const String &name, long, bool save = true);
    void var(const String &name, uint32_t, bool save = true);
    void var(const String &name, char, bool save = true);

  private:
    uiCallback ui;
    updateCallback upd;
    apiCallback api;
    buttonCallbackEvent btnCallbackFunc;

    bool _disableWiFiManagement = false;
    bool _useArduinoOta = false;

    void wifiEvent();
    void wifiSTA();
    void wifiAP();
    void wifiStart();
    uint8_t _wifiMode = 1;
    uint32_t _connectingTimer = 0;
    uint32_t _waitTimeForConnection = 0;

    bool _start = true;
    uint32_t _upTime = 0;
    uint32_t _runTimer = 0;
    bool _sendingToWeb = false;
    bool _useGauge = false;
    bool _useChart = false;

    void http();

    bool _debug = false;
    bool _espNeedReboot = false;

    bool _AutoSaveStatus = false;
    uint32_t _AutoSaveTimer = 0;
    void defaultWifi(uint8_t, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc);
    void cfgLoad();
    void cfgAutoSave();
    void cfgPrint();
    void var_auto_save(bool);

    String _app_name;
    String *_project_version = nullptr;
    String *_WebAuthLogin = nullptr;
    String *_WebAuthPass = nullptr;
    String *_apiKey = nullptr;
    String *_homePage = nullptr;
    String *_eMail = nullptr;
    String *_telega = nullptr;

    String *_bufBtnui = nullptr;
    String *_bufUI = nullptr;
    String *_bufOpt = nullptr;
    String *_bufUld = nullptr;
    String *_bufWebUpdate = nullptr;
    String *_bufApiResponse = nullptr;
};

#endif
