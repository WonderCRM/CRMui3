#ifdef ESP32
#define FORMAT_FS_IF_FAILED true
#else
#define FORMAT_FS_IF_FAILED
#endif

#include "CRMui3.h"

void CRMui3::cfgSave() {
  if (_AutoSaveStatus && SPIFFS.begin()) {
    String cfg_str = String();
    serializeJson(cfg, cfg_str);
    File configFile = SPIFFS.open(F("/config.json"), "w");
    configFile.print(cfg_str);
    configFile.close();
    _AutoSaveStatus = false;
    deserializeJson(cfg, cfg_str);
    DBGLN(F("[CFG] Config SAVED"));
  }
}


void CRMui3::cfgLoad() {
  if (SPIFFS.begin(FORMAT_FS_IF_FAILED)) {
    if (SPIFFS.exists(F("/config.json"))) {
      File configFile = SPIFFS.open(F("/config.json"), "r");
      String cfg_str = configFile.readString();
      configFile.close();
      if (cfg_str != "") {
        deserializeJson(cfg, cfg_str);
      } else SPLN(F("[CFG] Failed to open or read config file\nLoaded default value"));
    } else SPLN (F("[CFG] Can't finded config.json file\nLoaded default value"));
    if (var(F("_L")) == F("null")) var(F("_L"), F("ru"));
    if (var(F("_C1")) == F("null")) var(F("_C1"), F("#0098d9"));
    if (var(F("_C2")) == F("null")) var(F("_C2"), F("#ffffff"));
    ui();
    cfgPrint();
    cfgSave();
  } else SPLN (F("\n\n[CFG] Can't initialize SPIFFS"));
}


void CRMui3::cfgAutoSave() {
  if (_AutoSaveStatus && millis() - _AutoSaveTimer >= AUTO_SAVE_TIME_INTERVAL) {
    cfgSave();
  }
}


void CRMui3::cfgPrint() {
  if (_debug) {
    String buf = String();
    serializeJson(cfg, buf);
    SP(F("CFG: "));
    SPLN(buf);
  }
}


void CRMui3::cfgDelete() {
  if (!SPIFFS.begin(FORMAT_FS_IF_FAILED)) SPLN(F("Mount SPIFFS error. Format..."));
  else {
    SP(F("[CFG] Format SPIFFS, please wait..."));
    SPIFFS.format();
  }
  WiFi.disconnect(true);
  WiFi.softAPdisconnect(true);
  SPLN(F("\tDone\nESP Reboot!"));
  ESP.restart();
}


void CRMui3::var_auto_save(bool save) {
  if (save) {
    _AutoSaveStatus = true;
    _AutoSaveTimer = millis();
  }
}


String CRMui3::var(const String &name) {
  return cfg[name];
}

void CRMui3::var(const String &name, const String &value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, float value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, double value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, int value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, long value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, uint32_t value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, char value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}
