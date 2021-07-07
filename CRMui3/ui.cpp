#include "CRMui3.h"


/*
  typedef struct {
  const String &id;
  const String &label;
  String defaultValue;
    {
      {"label", "value"},
      ...
      до 50-ти
    }
  } Select;
*/
void CRMui3::select(Select item) {
  if (var(item.id) == "null") var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"o\":[";
  for (uint8_t i = 0; i < 50; i++) {
    if (item.options[i][0] != "") {
      if (!_buf.endsWith(",") && !_buf.endsWith("[")) _buf += ",";
      _buf += "[\"" + item.options[i][0] + "\",\"" + item.options[i][1] + "\"]";
    } else break;
  }
  _buf += "],\"id\":\"" + item.id + "\",";
  _buf += "\"t\":" + String(INPUT_SELECT) + ",";
  _buf += "\"l\":\"" + item.label + "\"}]";
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String d;
  String s;
  } Input;
*/
void CRMui3::input(Input item) {
  if (item.type != INPUT_BUTTON && item.type != INPUT_WIFI && var(item.id) == "null") var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"t\":" + String(item.type);
  _buf += ",\"id\":\"" + item.id + "\"";
  if (item.d != "") _buf += ",\"d\":\"" + item.d + "\"";
  if (item.s != "") _buf += ",\"s\":" + item.s;
  if (item.type == INPUT_BUTTON && item.defaultValue != "") _buf += ",\"p\":\"" + item.defaultValue + "\"";
  _buf += ",\"l\":\"" + item.label + "\"}]";
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String def;
  String c;
  String d;
  } Output;
*/
void CRMui3::output(Output item) {
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"t\":" + String(item.type);
  _buf += ",\"id\":\"" + item.id + "\"";
  _buf += ",\"l\":\"" + item.label + "\"";
  _buf += ",\"d\":\"" + item.def + "\"";
  if (item.c != "") _buf += ",\"c\":\"" + item.c + "\"";
  _buf += "}]";
}


/*
  typedef struct {
  const String &id;
  const String &label;
  float defaultValue;
  float min;
  float max;
  float step;
  String unit;
  } Range;
*/
void CRMui3::range(Range item) {
  if (var(item.id) == "null") var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"t\":" + String(INPUT_RANGE) + ",";
  _buf += "\"id\":\"" + item.id + "\",";
  _buf += "\"mn\":" + String(item.min) + ",";
  _buf += "\"mx\":" + String(item.max) + ",";
  _buf += "\"s\":" + String(item.step) + ",";
  _buf += "\"u\":\"" + item.unit + "\",";
  _buf += "\"l\":\"" + item.label + "\"}]";
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String icon;
  String color;
  bool newGroup;
  } Card;
*/
void CRMui3::card(Card item) {
  if (item.type == CARD_CHECKBOX && var(item.id) == "null") var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"t\":" + String(item.type);
  _buf += ",\"id\":\"" + item.id + "\"";
  _buf += ",\"l\":\"" + item.label + "\"";
  if (item.type != CARD_CHECKBOX) _buf += ",\"v\":\"" + item.defaultValue + "\"";
  if (item.icon != "") _buf += ",\"i\":\"" + item.icon + "\"";
  if (item.color != "") _buf += ",\"c\":\"" + item.color + "\"";
  if (item.newGroup) _buf += ",\"n\":1";
  _buf += "}]";
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String height;
  String color;
  String labelLine;
  String valueLine;
  } Chart;
*/
void CRMui3::chart(Chart item) {
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"t\":" + String(item.type);
  _buf += ",\"id\":\"" + item.id + "\"";
  _buf += ",\"l\":\"" + item.label + "\"";
  if (item.labelLine != "") _buf += ",\"a\":" + item.labelLine + "";
  if (item.valueLine != "") _buf += ",\"b\":" + item.valueLine + "";
  if (item.color != "") _buf += ",\"c\":\"" + item.color + "\"";
  if (item.height != "") _buf += ",\"h\":\"" + item.height + "\"";
  _buf += "}]";
}


void CRMui3::wifiForm(uint8_t mode, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc) {
  if (_start) {
    defaultWifi(mode, ap_ssid, ap_pass, ssid, pass, wtc);
    return;
  }
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += "{\"t\":" + String(INPUT_WIFI) + "}]";
}


void CRMui3::page(const String &pageName) {
  if (_start) return;
  if (!_buf.endsWith(",") && !_buf.endsWith("[")) _buf += ",";
  _buf += "[{\"m\":\"" + pageName + "\"}]";
}
