#include "SrcClientCallbacks.h"
#include "lwip/opt.h"
#include "config.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

void destClientConnect(AsyncClient *destClient, const char *data);

void handleSourceData(void *client, AsyncClient *sourceClient, void *d, size_t len)
{
    const char *data = (const char *)d;

    DEBUG_SERIAL.printf("Data received from sourceClient %s\n", sourceClient->remoteIP().toString().c_str());

    AsyncClient *destClient = (AsyncClient *)client;

    if (strncmp("CONNECT", data, 7) == 0)
    {
        DEBUG_SERIAL.println("'CONNECT' request received!");
        destClientConnect(destClient, data);
    }
    else if (destClient->connected())
    {
        DEBUG_SERIAL.println("Proxying request...");

        sourceClient->ackLater();
        destClient->onAck([](void *client, AsyncClient *destClient, size_t len, uint32_t time)
                          {
              DEBUG_SERIAL.printf("Dest client got ack for %d bytes\n", len);

    AsyncClient *sourceClient = (AsyncClient *)client;
    // Ack data from source now, after the dest has sent us an ACK.
    sourceClient->ack(len);
    
    // If there's any data in the buffer
    if(destClient->space() < TCP_SND_BUF)
      destClient->send(); },
                          sourceClient);

        DEBUG_SERIAL.printf("Processing source request... Params: len=%d; space: %d\n", len, destClient->space());
        // Copy the data into the other client
        // as this packet will be freed after this call
        destClient->write(data, len, 1);
    }
    else
    {
        DEBUG_SERIAL.println("No tunnel to destination!");
    }
}

void handleSourceDisconnect(void *client, AsyncClient *sourceClient)
{
    AsyncClient *destClient = (AsyncClient *)client;

    if (destClient)
    {
        destClient->close(true);
        destClient->free();
        delete destClient;
    }
    DEBUG_SERIAL.printf("Client %s disconnected \n", sourceClient->remoteIP().toString().c_str());
}

void handleSourceTimeOut(void *arg, AsyncClient *sourceClient, uint32_t time)
{
    DEBUG_SERIAL.printf("Source client ACK timeout ip: %s \n", sourceClient->remoteIP().toString().c_str());
}

void handleSourceError(void *arg, AsyncClient *sourceClient, int8_t error)
{
    DEBUG_SERIAL.printf("Connection error %s from source client %s \n", sourceClient->errorToString(error), sourceClient->remoteIP().toString().c_str());
}

void destClientConnect(AsyncClient *destClient, const char *data)
{
    // Default port
    uint16_t port = DEFAULT_HOST_PORT;

    char *host_start = NULL;
    char *port_start = NULL;
    char *host_end = NULL;

    char port_str[5];

    // Find host start
    host_start = strstr(data, "Host: ");

    // Can't do much if no host found
    if (!host_start)
    {
        return;
    }

    // Add the "Host: " index
    host_start = host_start + 6;
    // Find the end of the host
    host_end = strstr(host_start, "\r\n");
    // Find if a port was provided
    port_start = strstr(host_start, ":") + 1; // skip the ':'

    // Can't do much without figuring the host end
    if (!host_end)
    {
        return;
    }

    // If a port was given, use it
    if (port_start && port_start < host_end)
    {
        strncpy(port_str, port_start, host_end - port_start);
        port = atoi(port_str);
        host_end = port_start - 1; // consider the addition above
    }

    // We discard this request anyway
    // so just overwrite the host end
    // to NULL terminate the host string.
    *host_end = '\0';

    DEBUG_SERIAL.printf("Will create tunnel for: %s:%d\n", host_start, port);
    destClient->connect(host_start, port);
}