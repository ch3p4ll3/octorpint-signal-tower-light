#include "mqtt_task.h"

#include <ArduinoLog.h>
#include <config.h>
#include <status.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
TaskHandle_t mqttTaskHandle = NULL;

void mqttTask(void *pvParameters) {
  Config *args = static_cast<Config *>(pvParameters);

  Log.info("Initializing PubSubClient\n");

  mqttClient.setServer(args->mqtt.host.c_str(), args->mqtt.port);

  mqttClient.setCallback(mqtt_callback);

  for (;;) {
    if (!mqttClient.connected()) {
      Log.info("Connecting to broker\n");

      if (!args->mqtt.password.isEmpty() && !args->mqtt.username.isEmpty()) {
        mqttClient.connect("OctoSignal", args->mqtt.username.c_str(),
                           args->mqtt.password.c_str());
      } else {
        mqttClient.connect("OctoSignal");
      }

      for (String topic : args->mqtt.topics) {
        mqttClient.subscribe(topic.c_str());
      }
    }
    mqttClient.loop();
    vTaskDelay(100 / portTICK_PERIOD_MS); // run ~10 times/second
  }
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  Log.info(F("Message arrived [%s]\n"), topic);

  JsonDocument newCfg;
  DeserializationError err = deserializeJson(newCfg, payload, length);

  Log.info("Received JSON: ");
  serializeJsonPretty(newCfg, Serial);

  Log.info(F("New printer state: %s\n"),
           newCfg["state_string"].as<const char *>());

  setJobStatus(newCfg["state_string"]);
}