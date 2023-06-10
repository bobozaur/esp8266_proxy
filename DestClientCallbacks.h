#include <ESPAsyncTCP.h>

void handleDestConnect(void *client, AsyncClient *destClient);
void handleDestData(void *client, AsyncClient *destClient, void *d, size_t len);
void handleDestDisconnect(void *arg, AsyncClient *destClient);
void handleDestTimeOut(void *arg, AsyncClient *client, uint32_t time);
void handleDestError(void *arg, AsyncClient *client, int8_t error);