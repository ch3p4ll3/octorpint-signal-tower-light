#include <config.h>
#include <status.h>
#include "mqtt.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
TaskHandle_t mqttTaskHandle = NULL;

void mqttTask(void *pvParameters)
{
    Config *args = static_cast<Config *>(pvParameters);

    mqttClient.setServer(args->mqtt.host.c_str(), args->mqtt.port);

    mqttClient.setCallback(mqtt_callback);

    for (;;)
    {
        if (!mqttClient.connected())
        {
            if (!args->mqtt.password.isEmpty() && !args->mqtt.username.isEmpty())
            {
                mqttClient.connect("TorreESPClient", args->mqtt.username.c_str(), args->mqtt.password.c_str());
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
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    setJobStatus("jobStatus");
}