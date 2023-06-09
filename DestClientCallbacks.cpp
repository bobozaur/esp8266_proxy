#include "DestClientCallbacks.h"
#include "config.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

void handleSourceAck(void *client, AsyncClient *sourceClient, size_t len, uint32_t time);

void handleDestConnect(void *client, AsyncClient *destClient)
{
  DEBUG_SERIAL.println("Dest client connected!");

  AsyncClient *sourceClient = (AsyncClient *)client;
  sourceClient->add("HTTP/1.1 200 Connection Established\r\n\r\n", 40);
  sourceClient->send();
}

void handleDestData(void *client, AsyncClient *destClient, void *data_initial, size_t len)
{
  const char *data = (const char *)data_initial;
  DEBUG_SERIAL.printf("Data received from destClient %s\n", destClient->remoteIP().toString().c_str());

  AsyncClient *sourceClient = (AsyncClient *)client;

  destClient->ackLater();
  sourceClient->onAck(&handleSourceAck, destClient);

  DEBUG_SERIAL.printf("Processing Dest response... Params: len=%d; space: %d\n", len, sourceClient->space());
  sourceClient->write(data, len, 1);
}

void handleDestDisconnect(void *arg, AsyncClient *destClient)
{
  DEBUG_SERIAL.printf("Dest client %s disconnected \n", destClient->remoteIP().toString().c_str());
}

void handleDestTimeOut(void *arg, AsyncClient *client, uint32_t time)
{
  DEBUG_SERIAL.printf("Dest client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
}

void handleDestError(void *arg, AsyncClient *client, int8_t error)
{
  DEBUG_SERIAL.printf("Connection error %s from dest client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());
}

void handleSourceAck(void *client, AsyncClient *sourceClient, size_t len, uint32_t time)
{
  DEBUG_SERIAL.printf("Source client got ack for %d bytes\n", len);

  AsyncClient *destClient = (AsyncClient *)client;
  // Ack data from dest now, after the source has sent us an ACK.
  destClient->ack(len);
}