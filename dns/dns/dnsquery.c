// Module: dnsquery.c
//
// Description:
//    This sample illustrates how to use the WSALookupServiceXXX
//    functions with the DNS name space. Because the DNS name
//    space is static, we can't dynamically register our services
//    with it; however, we can query it using these functions.
//    This sample shows how to do this using the special GUID
//    which corresponds to the gethostbyname() function.
//
// Compile:
//    cl dnsquery.c ws2_32.lib
//
// Command Line Arguments/Parameters
//    dnsquery.exe hostname
//      hostname       Name of host to resolve
//
#include <winsock2.h>
#include <ws2tcpip.h>

#include <SvcGuid.h>

#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib,"Ws2_32.lib")
#define RNR_BUFFER_SIZE (sizeof(WSAQUERYSET) + sizeof(struct hostent) + 1024)

VOID FixList(PCHAR ** List, PCHAR Base)
{
    if (*List)
    {
        PCHAR * Addr;

        Addr = *List = (PCHAR *)( ((DWORD)*List + Base) );
        while(*Addr)
        {
            *Addr = (PCHAR)(((DWORD)*Addr + Base));
            Addr++;
        }
    }
}

VOID UnpackHostEnt(struct hostent * hostent)
{
    PCHAR pch;

    pch = (PCHAR)hostent;

    if(hostent->h_name)
    {
        hostent->h_name = (PCHAR)((DWORD)hostent->h_name + pch);
    }
    FixList(&hostent->h_aliases, pch);
    FixList(&hostent->h_addr_list, pch);
}

int main(int argc, char **argv)
{
    WSADATA        wsd;
    WSAQUERYSET   *qs=NULL;
    char           buff[RNR_BUFFER_SIZE];
    GUID           HostnameGuid = SVCID_INET_HOSTADDRBYNAME;
    HANDLE         hRnr;
    AFPROTOCOLS    afproto[2] = { {AF_INET, IPPROTO_UDP}, 
                                  {AF_INET, IPPROTO_TCP} };
    DWORD          dwLength=RNR_BUFFER_SIZE;
    int            ret, i;
    LPBLOB         pvRet=NULL;
    HOSTENT       *hp=NULL;
    SOCKADDR_IN    addr;

    if (argc != 2)
    {
        printf("usage: %s hostname\n", argv[0]);
        return -1;
    }

    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("WSAStartup() failed: %d\n", GetLastError());
        return -1;
    }

    qs = (WSAQUERYSET *)buff;

    memset(qs, 0, sizeof(*qs));
    qs->dwSize = sizeof(WSAQUERYSET);
    qs->lpszServiceInstanceName = argv[1]; // name to resolve
    qs->lpServiceClassId = &HostnameGuid;
    qs->dwNameSpace = NS_ALL;              // or ND_DNS
    qs->dwNumberOfProtocols = 2;           // TCP and UDP
    qs->lpafpProtocols = afproto;
    
    ret = WSALookupServiceBegin(qs, LUP_RETURN_BLOB | LUP_RETURN_NAME,
                &hRnr);
    if (ret == NO_ERROR)
    {
        ret = WSALookupServiceNext(hRnr, 0, &dwLength, qs);
        if (ret != NO_ERROR)
        {
            if (WSAGetLastError() == WSASERVICE_NOT_FOUND)
            {
                printf("No such name found!\n");
                WSALookupServiceEnd(hRnr);
                return 0;
            }
            printf("WSALookupServiceNext() failed: %d\n",
                WSAGetLastError());
        }
        WSALookupServiceEnd(hRnr);
    }

    if (qs->lpBlob == NULL)
        return -1;

    hp = (HOSTENT *)LocalAlloc(LPTR, qs->lpBlob->cbSize);
    if (!hp)
    {
        printf("LocalAlloc() failed: %d\n", GetLastError());
        return -1;
    }
    memcpy(hp, qs->lpBlob->pBlobData, qs->lpBlob->cbSize);

    UnpackHostEnt(hp);

    for(i=0; hp->h_addr_list[i] ;i++)
    {
        memcpy(&addr.sin_addr, hp->h_addr_list[i], hp->h_length);
        printf("%s IP: %s\n", argv[1], inet_ntoa(addr.sin_addr));
    }
    for(i=0; hp->h_aliases[i] ;i++)
        printf("Alias: %s\n", hp->h_aliases[i]);

    LocalFree(hp);

    WSACleanup();
	getchar();
    return 0;
}
