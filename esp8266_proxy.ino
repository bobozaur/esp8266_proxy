#include "config.h"
#include "DestClientCallbacks.h"
#include "SrcClientCallbacks.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

/* server events */
static void handleNewClient(void *arg, AsyncClient *sourceClient)
{
  if (sourceClient == NULL)
  {
    return;
  }

  DEBUG_SERIAL.printf("New client has been connected to server, ip: %s\n", sourceClient->remoteIP().toString().c_str());

  AsyncClient *destClient = new AsyncClient();
  destClient->setNoDelay(true);

  // register dest client events
  destClient->onConnect(&handleDestConnect, sourceClient);
  destClient->onData(&handleDestData, sourceClient);
  destClient->onDisconnect(&handleDestDisconnect, NULL);
  destClient->onTimeout(&handleDestTimeOut, NULL);
  destClient->onError(&handleDestError, NULL);

  // register source client events
  sourceClient->onData(&handleSourceData, destClient);
  sourceClient->onDisconnect(&handleSourceDisconnect, destClient);
  sourceClient->onTimeout(&handleSourceTimeOut, NULL);
  sourceClient->onError(&handleSourceError, NULL);
}

void setup()
{
  DEBUG_SERIAL.begin(115200);
  delay(20);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    DEBUG_SERIAL.printf("WiFi Failed!\n");
    return;
  }

  DEBUG_SERIAL.print("IP Address: ");
  DEBUG_SERIAL.println(WiFi.localIP());

  AsyncServer *server = new AsyncServer(TCP_PORT);
  server->setNoDelay(true);
  server->onClient(&handleNewClient, server);
  server->begin();
}

void loop() {}