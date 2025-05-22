# OctoPrint Signal Tower Light

An ESP32-based signal tower light for your 3D printer that connects to an OctoPrint instance using either the OctoPrint REST API or MQTT via a plugin. The device displays printer states visually using a multi-color light tower.

## Table of contents
- [OctoPrint Signal Tower Light](#octoprint-signal-tower-light)
  - [Table of contents](#table-of-contents)
  - [Features](#features)
  - [Hardware Requirements](#hardware-requirements)
  - [Setup Instructions](#setup-instructions)
  - [Configuration (`config.json`)](#configuration-configjson)
    - [Detailed Explanation of Settings](#detailed-explanation-of-settings)
      - [WiFi Settings](#wifi-settings)
      - [Access Point WiFi Settings](#access-point-wifi-settings)
      - [MQTT Settings](#mqtt-settings)
      - [REST API Settings](#rest-api-settings)
      - [Lights Configuration](#lights-configuration)
      - [States and Behavior](#states-and-behavior)
      - [Use MQTT Flag](#use-mqtt-flag)
  - [How it Works](#how-it-works)
  - [Contribution](#contribution)


## Features

* Connects to OctoPrint via REST API or MQTT.
* Configurable RGB or multi-light signal tower.
* Supports multiple printer states with customizable blinking patterns.
* Runs on ESP32 using PlatformIO.
* Offers an Access Point mode for initial setup or fallback.

## Hardware Requirements

* ESP32 development board
* Signal tower light with at least 3 separate controllable lights (e.g., Green, Yellow, Red)
* Appropriate wiring and power supply


## Setup Instructions

1. **Clone the repository**
   Clone this project to your local machine.

2. **Prepare configuration**
   Navigate to the `data/` folder and rename `config.json.example` to `config.json`.

3. **Edit the configuration**
   Open `config.json` and fill out the settings as explained below.

4. **Build and flash**
   Use PlatformIO to build and upload the firmware to your ESP32.

5. **Connect your signal tower**
   Wire your signal tower LEDs to the GPIO pins specified in the config.


## Configuration (`config.json`)

The `config.json` file controls all network, MQTT, REST, and light behavior settings.

```json
{
  "wifi": {
    "ssid": "",
    "password": ""
  },
  "ap_wifi": {
    "ssid": "octoprint-signal-tower",
    "password": "octosignal"
  },
  "mqtt": {
    "host": "",
    "port": 1883,
    "topics": [
      "octoPrint/event/PrinterStateChanged"
    ],
    "username": "",
    "password": ""
  },
  "rest": {
    "url": "",
    "apiKey": ""
  },
  "lights": [
    {
      "name": "Green",
      "pin": 4,
      "reversed": false
    },
    {
      "name": "Yellow",
      "pin": 3,
      "reversed": false
    },
    {
      "name": "Red",
      "pin": 2,
      "reversed": false
    }
  ],
  "states": {
    "Printing": [
      {
        "name": "Green",
        "blink": 500
      }
    ],
    "Pausing": [
      {
        "name": "Yellow",
        "blink": 250
      }
    ],
    "Paused": [
      {
        "name": "Yellow",
        "blink": 1000
      }
    ],
    "Error": [
      {
        "name": "Red",
        "blink": 500
      }
    ],
    "Complete": [
      {
        "name": "Green",
        "blink": 0
      }
    ],
    "Offline": [
      {
        "name": "Red",
        "blink": 0
      }
    ],
    "Operational": [
      {
        "name": "Green",
        "blink": 0
      }
    ],
    "Connecting": [
      {
        "name": "Green",
        "blink": 500
      }
    ]
  },
  "useMqtt": false
}
```

### Detailed Explanation of Settings

#### WiFi Settings

```json
"wifi": {
  "ssid": "",
  "password": ""
}
```

* **ssid**: The name of your WiFi network for the ESP32 to connect to.
* **password**: The WiFi network password.

This is the primary WiFi your ESP32 will use to connect to your network and reach the OctoPrint server.


#### Access Point WiFi Settings

```json
"ap_wifi": {
  "ssid": "octoprint-signal-tower",
  "password": "octosignal"
}
```

* **ssid**: The SSID broadcasted by the ESP32 when in Access Point mode.
* **password**: The password to connect to this AP.

If the ESP32 cannot connect to your main WiFi (above), it falls back to this Access Point mode so you can connect directly to it for configuration or troubleshooting.


#### MQTT Settings

```json
"mqtt": {
  "host": "",
  "port": 1883,
  "topics": [
    "octoPrint/event/PrinterStateChanged"
  ],
  "username": "",
  "password": ""
}
```

* **host**: IP address or hostname of your MQTT broker.
* **port**: MQTT broker port (default 1883).
* **topics**: MQTT topic(s) to subscribe to for OctoPrint events, typically from the OctoPrint `MQTT` plugin.
* **username**: MQTT username if authentication is required.
* **password**: MQTT password if authentication is required.

Use these settings if you want the ESP32 to listen for printer status via MQTT.

#### REST API Settings

```json
"rest": {
  "url": "",
  "apiKey": ""
}
```

* **url**: The base URL of your OctoPrint REST API, e.g., `http://octopi.local`.
* **apiKey**: The API key generated in OctoPrint (under Settings > API).

Use these settings if you want the ESP32 to poll or connect via REST API instead of MQTT.

#### Lights Configuration

```json
"lights": [
  {
    "name": "Green",
    "pin": 4,
    "reversed": false
  },
  {
    "name": "Yellow",
    "pin": 3,
    "reversed": false
  },
  {
    "name": "Red",
    "pin": 2,
    "reversed": false
  }
]
```

* **name**: Logical name of the light (e.g., Green, Yellow, Red).
* **pin**: ESP32 GPIO pin connected to this light.
* **reversed**: Set to `true` if the light logic is inverted (e.g., active-low LED).

Each entry defines a controllable light in the signal tower.

#### States and Behavior

```json
"states": {
  "Printing": [
    {
      "name": "Green",
      "blink": 500
    }
  ],
  ...
}
```

* The keys (e.g., `"Printing"`, `"Error"`, `"Paused"`) correspond to OctoPrint printer states.
* Each state maps to an array of light behaviors.
* Each behavior has:

  * **name**: The light name to activate.
  * **blink**: Blink interval in milliseconds. `0` means solid ON without blinking.

When the printer changes state, the corresponding lights will turn on and blink according to these rules.

#### Use MQTT Flag

```json
"useMqtt": false
```

* Set to `true` to use MQTT for receiving printer states.
* Set to `false` to use REST API.

## How it Works

* On boot, the ESP32 tries to connect to your configured WiFi.
* If it fails, it enables AP mode for manual connection.
* Depending on the `useMqtt` flag, it either subscribes to MQTT topics or polls the OctoPrint REST API for printer state.
* When the printer state changes, the ESP32 activates the configured lights with their blinking patterns to visually signal printer status.
* The light control supports reversed logic for different types of LEDs.

## Contribution

Feel free to open issues or submit pull requests!
