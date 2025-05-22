#include "config.h"

bool parseConfigFromJson(JsonDocument &doc, Config &config) {
  // WiFi
  config.wifi.clear();
  if (doc["wifi"].is<JsonArray>()) {
    JsonArray wifiArray = doc["wifi"].as<JsonArray>();
    for (JsonObject wifiObj : wifiArray) {
      WifiConfig cred;
      cred.ssid = wifiObj["ssid"] | "";
      cred.password = wifiObj["password"] | "";
      if (!cred.ssid.isEmpty()) {
        config.wifi.push_back(cred);
      }
    }
  }

  if (doc["ap_wifi"].is<JsonObject>()) {
    JsonObject ap_wifi = doc["ap_wifi"].as<JsonObject>();
    config.ap_wifi.ssid = ap_wifi["ssid"] | "octoprint-signal-tower";
    config.ap_wifi.password = ap_wifi["password"] | "octosignal";
  }

  // MQTT
  if (doc["mqtt"].is<JsonObject>()) {
    JsonObject mqtt = doc["mqtt"].as<JsonObject>();
    config.mqtt.host = mqtt["host"] | "";
    config.mqtt.port = mqtt["port"] | 1883;
    config.mqtt.username = mqtt["username"] | "";
    config.mqtt.password = mqtt["password"] | "";

    if (mqtt["topics"].is<JsonArray>()) {
      for (JsonVariant t : mqtt["topics"].as<JsonArray>()) {
        config.mqtt.topics.push_back(t.as<String>());
      }
    }
  }

  // REST
  if (doc["rest"].is<JsonObject>()) {
    JsonObject rest = doc["rest"].as<JsonObject>();
    config.rest.url = rest["url"] | "";
    config.rest.apiKey = rest["apiKey"] | "";
  }

  // LIGHTS
  config.lights.clear();
  if (doc["lights"].is<JsonArray>()) {
    for (JsonObject light : doc["lights"].as<JsonArray>()) {
      LightConfig l;
      l.name = light["name"] | "";
      l.pin = light["pin"] | -1;
      l.reversed = light["reversed"] | false;
      config.lights.push_back(l);
    }
  }

  // STATES
  if (doc["states"].is<JsonObject>()) {
    JsonObject states = doc["states"].as<JsonObject>();
    for (JsonPair kv : states) {
      String stateName = kv.key().c_str();
      if (!kv.value().is<JsonArray>())
        continue;
      std::vector<StateLight> lightsInState;
      for (JsonObject obj : kv.value().as<JsonArray>()) {
        StateLight sl;
        sl.name = obj["name"] | "";
        sl.blink = obj["blink"] | -1;
        lightsInState.push_back(sl);
      }
      config.states[stateName] = lightsInState;
    }
  }

  config.useMqtt = doc["useMqtt"] | false;

  return true;
}
