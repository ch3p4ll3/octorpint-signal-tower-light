#include <config.h>
#include <status.h>
#include "mqtt.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
TaskHandle_t mqttTaskHandle = NULL;

void mqttTask(void *pvParameters)
{
    Config *args = static_cast<Config *>(pvParameters);

    Serial.println("Initializing PubSubClient");

    mqttClient.setServer(args->mqtt.host.c_str(), args->mqtt.port);

    mqttClient.setCallback(mqtt_callback);

    for (;;)
    {
        if (!mqttClient.connected())
        {
            Serial.println("Connecting to broker");
            
            if (!args->mqtt.password.isEmpty() && !args->mqtt.username.isEmpty())
            {
                mqttClient.connect("OctoSignal", args->mqtt.username.c_str(), args->mqtt.password.c_str());
            }
            else {
                mqttClient.connect("OctoSignal");
            }

            for (String topic: args->mqtt.topics){
                mqttClient.subscribe(topic.c_str());
            }
        }
        mqttClient.loop();
        vTaskDelay(100 / portTICK_PERIOD_MS); // run ~10 times/second
    }
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    JsonDocument newCfg;
    DeserializationError err = deserializeJson(newCfg, payload, length);

    Serial.println("Received JSON:");
    serializeJsonPretty(newCfg, Serial);

    setJobStatus(newCfg["state_string"]);
}