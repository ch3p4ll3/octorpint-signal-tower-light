#include <Arduino.h>
#include <vector>
#include <map>
#include <config.h>

struct DynamicLight {
  String name;
  int pin;
  bool reversed;
  int blinkIntervalMs;
  bool currentState;
  unsigned long lastToggle;
};

extern std::vector<DynamicLight> lights;
extern TaskHandle_t lightsTaskHandle;

void lightTask(void *pvParameters);
void updateLightsFromState(const String& state);
void registerLightsFromConfig(Config* config);