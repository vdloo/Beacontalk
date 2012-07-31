// Beacontalk - peer to peer chat-program that sends data over Wi-Fi without associations.
// Copyright (C) 2012 Rick van de Loo

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>

#include "beaconpacket.h"

int get_beaconpacket()
{
	/* defines needed things for pcap */
	pcap_t *handle;
	struct pcap_pkthdr header;
	const u_char * packet;
	char errbuf[2048];

	/* defines other variables */
	int k, l, identified, start, end, intlength, checkdouble;
	char charlength[6];
	char messagebuffer[512][1];

	/* opening pcap session */
	handle = pcap_open_live(dev, 1024, 0, 1024, errbuf);
	if (handle == NULL){
		printf("Couldn't open device %s\n", errbuf);
		exit(1);
	}

	/* callback from pcap_loop */
	void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
		/* drop everything that does not contain "x0\x11\x0\x0\x5\x73\x74\x6F\x6E\x65" */
		identified = 0;
		for (k = 0; k < 300; k++){
			if (identified == 0 && packet[k] == 's' && packet[k+1] == 't' && packet[k+2] == 'o' && packet[k+3] == 'n' && packet[k+4] == 'e'){
				identified = 1;	
			}
		}
		if (identified){
			start = 0;
			intlength = 0;
			for (k = 1; k < 1024; k++){
				if (start == 0 && packet[k] == 's' && packet[k+1] == 'i' && packet[k+2] == 'z' && packet[k+3] == 'e'){
					start = k;
				}
			}
			if (start > 0){
				for (k = 0; k < 5; k++){
					charlength[k] = packet[start+k+4];
				}
				intlength = atoi(charlength);
			}
			if (intlength > 0){
				l = 0;
				for (k = start+4+5+1; k <= start+4+5+intlength; k++){
					sprintf(messagebuffer[l], "%c", (unsigned char) packet[k]);
					l++;
				}
				strcpy(text[0], messagebuffer);
				/* clears any double messages, a guid would be nicer here */ 
				checkdouble = 0;
				for (k = 1; k < 20; k++){
					if (strcmp(text[0], text[k]) == 0){
						checkdouble = 1;
					}
				}
				if (checkdouble){
					text[0][0] = 0;
				}
				else{
					for (k = 5; k > 0; k--){
						if ( -1 == pcap_sendpacket(handle, packet, 1024))
							printf("sending package failed\n");
					}
				}
				shiftarray();
			}
		}
	}

	/* starting the pcap_loop */	
	pcap_loop(handle, -1, got_packet, NULL);
	return(0);
}

int send_beaconpacket(char data[512])
{

	char message[1024-107]; /* careful not to overflow the buffer here */
	strcpy(message, data);

	pcap_t *handle;

	char errbuf[512];
	handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if (handle == NULL){
		printf("Couldn't open device %s\n", errbuf);
		exit(1);
	}
	u_char buf[1024];
	char type[] = "\x0\x0\xd\x0\x4\x80\x2\x0\x2\x0\x0\x0\x0\x80\x0\x0\x0\xff\xff\xff\xff\xff\xff\xf8\xb7\x6f\x7b\xc2\x59\xf8\xb7\x6f\x7b\xc2\x59\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x64\x0\x11\x0\x0\x5\x73\x74\x6f\x6e\x65\x1\x4\x82\x84\x8b\x96\x3\x1\xc\x4\x6\x1\x2\x0\x0\x0\x0\x5\x4\x0\x1\x0\x0\xdd\x18\x0\x50\xf2\x1\x1\x0\x0\x50\xf2\x4\x1\x0\x0\x50\xf2\x4\x1\x0\x0\x50\xf2\x2\x0\x0"; /* hardcoded stone header beacon frame */
	/* adds the beacon frame */ 
	memcpy(buf, type, sizeof(type));
	char size[] = "size";
	char messagelen[6];
	sprintf(messagelen, "%.5d", strlen(message));
/*	printf("messagelen is %s\n", messagelen); */
	
	/* adds the size marker */
	memcpy(buf+sizeof(type), size, strlen(size));

	/*adds the size number */
	memcpy(buf+sizeof(type)+strlen(size), messagelen, 5);

	/* adds the message */
	memcpy(buf+sizeof(type)+strlen(size)+sizeof(messagelen), message, strlen(message));
	char end[] = "end"; /* ends the stone data */
	
	/* ends the stone data string */	
	memcpy(buf+sizeof(type)+strlen(size)+sizeof(messagelen)+strlen(message), end, strlen(end));

	
	/* stone frame:	[			packet			]
			[beacon frame][             stone		]
			[beacon frame][size][sizenr][    message   ][end]
	*/


	printf("%s\n",message);
	if ( -1 == pcap_sendpacket(handle, buf, 1024))
		printf("sending package failed\n");
	return(0);
}
