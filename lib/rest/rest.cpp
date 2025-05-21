#include "rest.h"

TaskHandle_t octoPrintAPITaskHandle = NULL;


void octoPrintAPITask(void *pvParameters)
{
    Config *args = static_cast<Config *>(pvParameters);
    HTTPClient http;
    JsonDocument jsonDoc;

    while (true)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            String url = String(args->rest.url) + "/api/job";

            http.begin(url);
            http.addHeader("X-Api-Key", args->rest.apiKey);

            int httpResponseCode = http.GET();

            if (httpResponseCode == 200)
            {
                String payload = http.getString();

                DeserializationError error = deserializeJson(jsonDoc, payload);
                if (!error)
                {
                    // extract print state
                    const char *state = jsonDoc["state"];
                    Serial.printf("OctoPrint job state: %s\n", state);

                    setJobStatus(state);
                }
                else
                {
                    Serial.println("JSON parse error");
                }
            }
            else
            {
                Serial.printf("HTTP GET failed, error: %d\n", httpResponseCode);
            }

            http.end();
        }
        else
        {
            Serial.println("WiFi not connected");
        }

        vTaskDelay(pollIntervalMs / portTICK_PERIOD_MS);
    }
}
