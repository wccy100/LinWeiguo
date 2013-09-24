#include "Device.h"

Device::~Device()
{
    /* At this point, we don't need any more the device list. Free it */
    pcap_freealldevs(alldevs);
}

int Device::obtainDeviceList()
{
	i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    /* Retrieve the device list from the local machine */
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &alldevs, errbuf) == -1)
	{
        fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
		return -1;
	}

    /* Print the list */
    for(d= alldevs; d != NULL; d= d->next)
        printf("%d. %s\n", ++i, d->description);
    
    if (i == 0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return -1;
    }

	return 0;
}

int Device::openAdapter()
{
	int inum;
	char errbuf[PCAP_ERRBUF_SIZE];
	printf("Enter the interface number (1-%d):",i);
    scanf_s("%d", &inum);
    
    if(inum < 1 || inum > i)
    {
        printf("\nInterface number out of range.\n");
        return -1;
    }
    
    /* Jump to the selected adapter */
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

    /* Open the device */
    if ( (adhandle= pcap_open(d->name,          // name of the device
                              65536,            // portion of the packet to capture
                                                // 65536 guarantees that the whole packet will be captured on all the link layers
                              PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
    {
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
        return -1;
    }
	return 0;
}

int Device::setFilter(const char *packet_filter)
{
	u_int netmask;
	struct bpf_program fcode;

    if (d->addresses != NULL)
        /* Retrieve the mask of the first address of the interface */
        netmask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        /* If the interface is without an address we suppose to be in a C class network */
        netmask=0xffffff; 


	//compile the filter
    if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
    {
        fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }
    
	//set the filter
    if (pcap_setfilter(adhandle, &fcode) < 0)
    {
        fprintf(stderr,"\nError setting the filter.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }
	return 0;
}

ULONG Device::getIp()
{
	pcap_addr_t *a;
	/* IP addresses */
	for(a=d->addresses;a;a=a->next)
	{
		if(AF_INET == a->addr->sa_family)
			if (a->addr)
				return ((struct sockaddr_in *)a->addr)->sin_addr.S_un.S_addr;
    }
	return NULL;
}

void Device::sendPacket(const BYTE *packet)
{
    /* Send down the packet */
    if (pcap_sendpacket(adhandle, packet, 60 /* size */) != 0)
    {
        fprintf(stderr,"\nError sending the packet: %s\n", pcap_geterr(adhandle));
        return;
    }
}

DWORD WINAPI Device::recvPacket(LPVOID lpParam)
{
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	timeval send[4], recv[4];
	int i=0, j=0;
	BYTE mac[6];
	ARP a;

    /* Retrieve the packets */
    while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
        if(res == 0)
            /* Timeout elapsed */
            continue;
        
		EthernetFrame recvEF((BYTE *)pkt_data);
		recvEF.macdata2Arp(a);
		if (htons(ARP_REQUEST) == recvEF.getArpOperationCode())
		{
			send[i] = header->ts;
			++i;
		}
		if (htons(ARP_REPLY) == recvEF.getArpOperationCode())
		{
			recv[j] = header->ts;
			++j;
		}

		if(4 == i && 4 == j)
		{
			for(int k=0; k<4; ++k)
			{
				cout << "Unicast reply from " << Common::dword2Char(recvEF.getArpSourceProtocolAddress()) << " ";
				memcpy(mac, recvEF.getArpSourceHardwareAddress(), 6);
				printf("[%02X:%02X:%02X:%02X:%02X:%02X]", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				cout << " ";
				cout << Common::timeSub(send[k],recv[k]) << "ms" << endl;
			}
			cout << "Sent 4 probes (1 broadcast(s)) Received 4 response(s)" << endl;
			break;
		}
    }
    
    if(res == -1){
        printf("Error reading the packets: %s\n", pcap_geterr(adhandle));
        return -1;
    }
	return 0;
}