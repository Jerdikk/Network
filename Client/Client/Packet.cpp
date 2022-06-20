#include "stdafx.h"
#include "Packet.h"
#include <string.h>

MYPACKET *pack(char* text)
{
    MYPACKET* P = new MYPACKET();
    
    
    P->p.packetType = 'C';

    P->p.packetSize = (unsigned char)strlen(text);
    for (int i = 0; i < P->p.packetSize; i++)
        P->p.body[i] = text[i];

    return P;
}

void decode(MYPACKET* pack, char *text)
{    
    try {
        for (size_t i = 0; i < pack->p.packetSize; i++)
        {
            text[i] = pack->p.body[i];
        }
        text[pack->p.packetSize] = 0;
    }    
    catch (...)
    {
        int yyy = 1;
    }
    
}

int packetSize(MYPACKET* pack)
{

    return pack->p.packetSize+2;
}

void nullPacket(MYPACKET* pack)
{
    for (size_t i = 0; i < 258; i++)
    {
        pack->r.buffer[i] = 0;
    }
}
