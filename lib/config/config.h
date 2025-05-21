#pragma once

#include <ArduinoJson.h>
#include <vector>
#include <Arduino.h>
#include <map>


struct LightConfig {
  String name;
  int pin;
  bool reversed;
};

struct StateLight {
  String name;
  int blink;
};

struct WifiConfig {
  String ssid;
  String password;
};

struct MqttConfig {
  String host;
  int port;
  std::vector<String> topics;
  String username;
  String password;
};

struct RestConfig {
  String url;
  String apiKey;
};

struct Config {
  WifiConfig wifi;
  WifiConfig ap_wifi;
  MqttConfig mqtt;
  RestConfig rest;
  std::vector<LightConfig> lights;
  std::map<String, std::vector<StateLight>> states;
  bool useMqtt;
};
