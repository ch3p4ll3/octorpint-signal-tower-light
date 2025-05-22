#include <ArduinoLog.h>
#include "octoprint_API_task.h"

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
            String url = String(args->rest.url) + "/api/printer?exclude=temperature,sd";

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
                    const char *state = jsonDoc["state"]["text"];
                    Log.info(F("New printer state: %s \n"), state);

                    setJobStatus(state);
                }
                else
                {
                    Log.error("JSON parse error\n");
                }
            }
            else
            {
                Log.error(F("HTTP GET failed, error: %d\n"), httpResponseCode);
            }

            http.end();
        }
        else
        {
            Log.error("WiFi not connected");
        }

        vTaskDelay(pollIntervalMs / portTICK_PERIOD_MS);
    }
}
