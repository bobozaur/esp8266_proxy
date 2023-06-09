#include <ESP8266WiFi.h>

// Macro for controlling serial debugging
#define DEBUG_SERIAL \
    if (DEBUG)       \
    Serial

#define DEBUG true // set to true for debug output, false for no debug output
#define SSID "YOUR_SSID"
#define PASSWORD "YOUR_PASSWORD"
#define TCP_PORT 3000

/*
Used if the CONNECT request does not contain a port.
The default HTTPS port should be fine for most use-cases,
but this allows configuration because... why not?
*/
#define DEFAULT_HOST_PORT 443