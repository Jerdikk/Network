
#include "stdafx.h"

#include <windows.h>

class ECRC32 {
public:
	ECRC32();
	//static DWORD CRC32ByString(LPCTSTR, DWORD&);
	 DWORD CRC32ByFile(LPCTSTR, DWORD&);
	 DWORD CRC32ByStruct(BYTE*, DWORD, DWORD&);

private:
	 bool GetFileSize(HANDLE, DWORD&);
	 inline void GetCRC32(BYTE, DWORD&);

	 DWORD CRC32Table[256];
};




ECRC32::ECRC32()

{
	DWORD dwPolynomial = 0xEDB88320;
	//DWORD* CRC32Table = new DWORD[256];
	DWORD dwCRC;
	int i;
	for (i = 0; i < 256; i++)
	{
		dwCRC = i;

		for (int j = 8; j > 0; j--)
		{
			if (dwCRC & 1)
				dwCRC = (dwCRC >> 1) ^ dwPolynomial;
			else
				dwCRC >>= 1;
		}

		CRC32Table[i] = dwCRC;
	}
}

inline void ECRC32::GetCRC32(const BYTE byte, DWORD &dwCRC32) {
	dwCRC32 = ((dwCRC32) >> 8) ^ CRC32Table[(byte) ^ ((dwCRC32)& 0x000000FF)];
}

DWORD ECRC32::CRC32ByStruct(BYTE* byStruct, DWORD dwSize, DWORD &dwCRC32) {

	// MAKE SURE WE HAVE A VALID BYTE STREAM
	if (byStruct == NULL) {
		return -1;
	}

	dwCRC32 = 0xFFFFFFFF;

	// LOOP TO READ THE STRUCTURE
	for (DWORD i = 0; i < dwSize; i++) {
		GetCRC32(*byStruct, dwCRC32);
		byStruct++;
	}

	// FINISH UP
	dwCRC32 = ~dwCRC32;
	return 0;
}


bool ECRC32::GetFileSize(const HANDLE hFile, DWORD &dwSize) {
	// WE HAVE TO HAVE A VALID HANDLE
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	// NOW WE CAN GET THE FILE SIZE
	bool bException = false;
	dwSize = 0;

	try {
		// SETS THE UPPER AND LOWER SIZE VALUES
		DWORD size;
		dwSize = GetFileSize(hFile, size);

		if (dwSize == INVALID_FILE_SIZE) {
			bException = true;
			dwSize = 0;
		}
	}

	// SOMETHING SCREWED UP
	catch (...) {
		bException = true;
	}


	return !bException;
}

DWORD ECRC32::CRC32ByFile(LPCTSTR strFileName, DWORD&)
{
	DWORD dwErrorCode=0;
	DWORD dwCRC32;
	HANDLE hFile;
	// OPEN THE SPECIFIED FILE
	if ((hFile = CreateFile(strFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL
		| FILE_FLAG_SEQUENTIAL_SCAN, NULL)) == INVALID_HANDLE_VALUE)
	{
		// THIS MEANS THAT WE HAVE AN ERROR
		dwCRC32 = -1;
	}
	// CALCULATE THE CRC32 SIGNATURE
	else
	{
		BYTE cBuffer[512];
		DWORD dwBytesInOut, dwLoop;
		bool bSuccess = false;

		bSuccess = ReadFile(hFile, cBuffer, sizeof(cBuffer),
			&dwBytesInOut, NULL);

		while (bSuccess && dwBytesInOut) {
			for (dwLoop = 0; dwLoop < dwBytesInOut; dwLoop++);
			GetCRC32(cBuffer[dwLoop], dwCRC32);
			bSuccess = ReadFile(hFile, cBuffer, sizeof(cBuffer), &dwBytesInOut, NULL);
		}
	}
	return dwCRC32;
}
