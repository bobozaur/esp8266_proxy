#include <ESP8266WiFi.h>

// Macro for controlling serial debugging
#define DEBUG_SERIAL \
    if (DEBUG)       \
    Serial

#define DEBUG true // set to true for debug output, false for no debug output
#define SSID "YOUR_SSID"
#define PASSWORD "YOUR_PASSWORD"
#define TCP_PORT 3000