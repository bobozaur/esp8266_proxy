#include <ESPAsyncTCP.h>

void handleSourceData(void *client, AsyncClient *sourceClient, void *d, size_t len);
void handleSourceDisconnect(void *client, AsyncClient *sourceClient);
void handleSourceTimeOut(void *arg, AsyncClient *client, uint32_t time);
void handleDestAck(void *client, AsyncClient *destClient, size_t len, uint32_t time);
void handleSourceError(void *arg, AsyncClient *client, int8_t error);
