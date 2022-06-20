#pragma once



struct Packet
{
	char packetType;
	unsigned char packetSize;
	char body[256];
};

struct RawPacket
{
	char buffer[258];
}; 

union MYPACKET
{
	Packet p;
	RawPacket r;

};

extern MYPACKET *pack(char* text);
extern void decode(MYPACKET* pack, char* text);
extern int packetSize(MYPACKET* pack);
extern void nullPacket(MYPACKET* pack);