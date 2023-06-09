# esp8266_proxy
An Arduino IDE project for an ESP8266 async HTTP proxy.

# Installation

- Install the [esp8266 Arduino Core](https://github.com/esp8266/Arduino#using-git-version) through the **Arduino IDE git version instructions**.
- Download the [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) library.
- Clone this repository in your sketchbook (usually something like `$HOME/Arduino`).
- Tweak the [TCP settings](#TCP-settings) to suit your network/setup.
- Tweak the `config.h` file to set the network/port/debug options.

You should now be able to compile and upload the code.

# How it works

The server will wait for a client (like a browser), which should start by sending a `CONNECT` HTTP request, signaling to create a TCP tunnel to the desired host in the request.

The host is extracted from the `CONNECT` request and another client (the proxy) is created, connecting to the host. Once connected, a response is sent to the first client, essentially telling it that the tunnel was established. 

The proxy is operational now, and additional request/response packets get piped between the two clients, postponing `ACK`s until the other party sends its own `ACK`.

This project was more of a fun/research/learning opportunity kind of thing for me. Nevertheless, if you do see any improvement area please feel free though to open a PR or even just an issue.

It works way better than I anticipated, and that's mostly due to the [lwIP](https://savannah.nongnu.org/projects/lwip/) stack and the `ESPAsyncTCP` library by **me-no-dev**. However, don't forget that this is running on an ESP8266 microcontroller. 

# TCP settings
The default TCP settings from the `esp8266 Arduino Core` get in the way of this working properly, essentially because the `TCP_WND` is greated than the `TCP_SND_BUF`. So the (actual) server will send more data than the proxy buffer can hold before it gets an `ACK` from its client, which is bad.

Also, the default lwIP Variant has a packet size (`TCP_MSS`) of 536, which is conservative.

- Consider using a different lwIP Variant, from `Tools -> lwIP Variant`. The additional features are not needed as far as I'm aware, so I just use `v2 Higher Bandwidth (no features)`. This increases `TCP_MSS` to 1460, the default Ethernet packet payload size.
- Locate the `esp8266 Arduino core` installation.
- Navigate to `hardware/esp8266com/esp8266/tools/sdk/lwip2/builder/glue-lwip/arduino`.
- Modify the `TCP_WND` and `TCP_SND_BUF` macros. They are already defined in relation to `TCP_MSS`, but `TCP_WND` might be bigger. We actually could use the opposite, having more room in the buffer to hold data while waiting for an `ACK` without the server flooding us with data. The settings that work best for me are: 
    ```
    #define TCP_WND  (4 * TCP_MSS)
    #define TCP_SND_BUF  (6 * TCP_MSS)
    ```
- Navigate to `hardware/esp8266com/esp8266/tools/sdk/lwip2` and run `make all install clean`. This will recompile the `lwIP2` libraries with the redefined macros.
- Recompile and upload the project
- Profit???

The instructions for modifying these macros are inspired from [this](https://arduino.stackexchange.com/a/90004/90867) StackExchange answer.