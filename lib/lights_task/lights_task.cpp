#include "lights_task.h"

#include <status.h>


TaskHandle_t lightsTaskHandle = NULL;
std::vector<DynamicLight> lights;


void registerLightsFromConfig(Config *config)
{
    lights.clear();

    for (LightConfig lightConfig : config->lights)
    {
        DynamicLight light = {
            lightConfig.name,
            lightConfig.pin,
            lightConfig.reversed,
            0,
            lightConfig.reversed,
            0};
        pinMode(light.pin, OUTPUT);
        lights.push_back(light);
    }
}

void updateLightsFromState(Config *config, const String &state)
{
    // First, disable all lights
    for (auto &light : lights)
    {
        light.blinkIntervalMs = -1;
    }

    // Check if the state exists in the map
    auto it = config->states.find(state);
    if (it == config->states.end())
        return;

    // it->second is the vector<StateLight> for the matched state
    for (const auto &stateLight : it->second)
    {
        for (auto &light : lights)
        {
            if (light.name == stateLight.name)
            {
                light.blinkIntervalMs = stateLight.blink;
                break;
            }
        }
    }
}

void handleBlink(DynamicLight &light)
{
    if (light.blinkIntervalMs == -1)
    {
        // Turn OFF light, respecting reversed logic
        digitalWrite(light.pin, light.reversed ? HIGH : LOW);
        return;
    }
    if (light.blinkIntervalMs == 0)
    {
        // Turn ON light, respecting reversed logic
        digitalWrite(light.pin, light.reversed ? LOW : HIGH);
        return;
    }

    if (millis() - light.lastToggle >= light.blinkIntervalMs)
    {
        light.currentState = !light.currentState;

        // Use reversed logic for blinking state
        if (light.reversed)
        {
            digitalWrite(light.pin, light.currentState ? LOW : HIGH);
        }
        else
        {
            digitalWrite(light.pin, light.currentState ? HIGH : LOW);
        }

        light.lastToggle = millis();
    }
}


void lightTask(void *pvParameters)
{
    Config *args = static_cast<Config *>(pvParameters);

    registerLightsFromConfig(args);
    for (;;)
    {
        updateLightsFromState(args, getJobStatus());
        for (auto &light : lights)
        {
            handleBlink(light);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
