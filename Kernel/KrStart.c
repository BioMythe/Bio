#include <stdint.h>

void KrStart(void* pFrameBuffer)
{
    uint8_t* fbuf = (uint8_t*) pFrameBuffer;
    *fbuf = 0xFF;

    while (1);
}
