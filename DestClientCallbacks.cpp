#include "DestClientCallbacks.h"
#include "lwip/opt.h"
#include "config.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

void handleDestConnect(void *client, AsyncClient *destClient)
{
  DEBUG_SERIAL.println("Dest client connected!");

  AsyncClient *sourceClient = (AsyncClient *)client;
  sourceClient->write("HTTP/1.1 200 Connection Established\r\n\r\n", 40);
}

void handleDestData(void *client, AsyncClient *destClient, void *d, size_t len)
{
  const char *data = (const char *)d;
  DEBUG_SERIAL.printf("Data received from destClient %s\n", destClient->remoteIP().toString().c_str());

  AsyncClient *sourceClient = (AsyncClient *)client;

  destClient->ackLater();
  sourceClient->onAck([](void *client, AsyncClient *sourceClient, size_t len, uint32_t time)
                      {
  DEBUG_SERIAL.printf("Source client got ack for %d bytes\n", len);

  AsyncClient *destClient = (AsyncClient *)client;
  // Ack data from dest now, after the source has sent us an ACK.
  destClient->ack(len);

  // If there's any data in the buffer
  if(sourceClient->space() < TCP_SND_BUF)
    sourceClient->send(); },
                      destClient);

  DEBUG_SERIAL.printf("Processing dest response... Params: len=%d; space: %d\n", len, sourceClient->space());
  // Copy the data into the other client
  // as this packet will be freed after this call
  sourceClient->write(data, len, 1);
}

void handleDestDisconnect(void *arg, AsyncClient *destClient)
{
  DEBUG_SERIAL.printf("Dest client %s disconnected \n", destClient->remoteIP().toString().c_str());
}

void handleDestTimeOut(void *arg, AsyncClient *destClient, uint32_t time)
{
  DEBUG_SERIAL.printf("Dest client ACK timeout ip: %s \n", destClient->remoteIP().toString().c_str());
}

void handleDestError(void *arg, AsyncClient *destClient, int8_t error)
{
  DEBUG_SERIAL.printf("Connection error %s from dest client %s \n", destClient->errorToString(error), destClient->remoteIP().toString().c_str());
}
