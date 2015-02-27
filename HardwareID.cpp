#include "stdafx.h"
#include "DiskInfo.h"
#include <WinSock2.h>
#include <Iphlpapi.h>
#include "Crc32Dynamic.h"
#include "Md5.h"
#pragma comment(lib, "Iphlpapi.lib")

bool GenerateHardwareID( DWORD &dwCrc )
{
	char szCPUBrandString[60];
	int iCPUInfo[4];
	std::string raw;

	__cpuid(iCPUInfo, 0x80000002);
	memset(szCPUBrandString, 0, sizeof(szCPUBrandString));
	memcpy(szCPUBrandString, iCPUInfo, sizeof(iCPUInfo));
	__cpuid(iCPUInfo, 0x80000003);
	memcpy(szCPUBrandString+16, iCPUInfo, sizeof(iCPUInfo));
	__cpuid(iCPUInfo, 0x80000004);
	memcpy(szCPUBrandString+32, iCPUInfo, sizeof(iCPUInfo));
	raw.append(szCPUBrandString);
	Log(("szCPUBrandString %s", szCPUBrandString));

	// Does not work without admin righs :-/
	//long lDiskCount = DiskInfo::GetDiskInfo().LoadDiskInfo();
	//Log(("lDiskCount %d", lDiskCount));
	//if( lDiskCount <= 0 )
	//	return false;

	//for( long count = 0; count < lDiskCount; count ++ ) {
	//	raw.append(DiskInfo::GetDiskInfo().SerialNumber(count));
	//	raw.append(DiskInfo::GetDiskInfo().ModelNumber(count));
	//}
	//DiskInfo::Destroy( );

	// Get adapter info
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG uOutBuf = 15000;
	ULONG uRet = 0;

	do {
		pAddresses = new IP_ADAPTER_ADDRESSES[uOutBuf];
		if( !pAddresses )
			return false;

		uRet = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_UNICAST|GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST|GAA_FLAG_SKIP_DNS_SERVER,
			NULL, pAddresses, &uOutBuf);
		if( uRet == ERROR_BUFFER_OVERFLOW ) {
			delete[] pAddresses;
			pAddresses = NULL;
			continue;
		}
		else {
			break;
		}
	} while( uRet == ERROR_BUFFER_OVERFLOW );

	if( uRet == NO_ERROR ) {
		PIP_ADAPTER_ADDRESSES pCurAddress = pAddresses;

		// Walk address list until we find one with a physical address
		// and append that to the raw hardware data
		while( pCurAddress ) {
			if( pCurAddress->PhysicalAddressLength != 0 ) {
				for( int i = 0; i < pCurAddress->PhysicalAddressLength; i++ ) {
					char p[256];
					sprintf(p, XStr( /*%.2X*/ 0x01, 0x04, 0x00, 0x252F305B ).c(), pCurAddress->PhysicalAddress[i]);
					raw.append(p);
				}
				break;
			}
			pCurAddress = pCurAddress->Next;
		}

		delete[] pAddresses;
		pAddresses = NULL;
	}
	else {
		Log(("Error while getting addresses"));
		delete[] pAddresses;
		pAddresses = NULL;
		return false;
	}

	raw = md5(raw);

	raw.append(XStr( /*{A24662BF-85C5-4924-B65B-D1A195E99B4B}*/ 0x0A, 0x26, 0x00, 0x7B403037, 0x32333445, 0x4E24323E, 0x4F38233B, 0x2923263E, 0x56232355, 0x355D2B5A, 0x2D242B5A, 0x19186017, 0x66580000 ).c()); // Unique GUID for this cheat. Needs to be appended before generating CRC32.
	Log(("Raw hardware id %s", raw.c_str()));
	CCrc32Dynamic crc;
	crc.Init();
	if( crc.StringCrc32(raw.c_str(), dwCrc) != NO_ERROR )
		return false;


	return true;
}
