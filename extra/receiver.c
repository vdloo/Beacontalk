#include <stdio.h>
#include <string.h>
#include <pcap.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
		
	/* checks for device */
	if (argc < 2){
		printf("enter interface as argument\n");
		return(0);
	}
	char *dev = argv[1];
	
	/* defines things needed for pcap */
	pcap_t *handle;
	struct pcap_pkthdr header;
	const u_char * packet;
	char errbuf[512];

	/* defines other stuff */
	int i, identified;
	int start;
	int end;
	char charlength[6];
	int intlength;

	
	/* opening pcap session */
	handle = pcap_open_live(dev, 1024, 0, 1024, errbuf);
	if (handle == NULL){
		printf("Couldn't open device %s\n", errbuf);
		return(1);
	}

	/* callback from pcap_loop */
	void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
		/* drop everything that does not contain "x0\x11\x0\x0\x5\x73\x74\x6F\x6E\x65" */
		identified = 0;
		for (i = 0; i < 300; i++){
			if (identified == 0 && packet[i] == 's' && packet[i+1] == 't' && packet[i+2] == 'o' && packet[i+3] == 'n' && packet[i+4] == 'e'){
				identified = 1;	
			}
		}
		if (identified){
			/* looks for beginning of datastring and it's size */
			start = 0;
			intlength = 0;
			for (i = 1; i < 1024; i++){
				if (start == 0 && packet[i] == 's' && packet[i+1] == 'i' && packet[i+2] == 'z' && packet[i+3] == 'e'){
					start = i;
				}
			}
			if (start > 0){
				for (i = 0; i < 5; i++){
					charlength[i] = packet[start+i+4];
				}
				intlength = atoi(charlength);
			}

			/* print output */
			if (intlength > 0){
				for (i = start+9; i <= start+9+intlength; i++){
					fprintf(stdout, "%c", packet[i]);
				}
			}
		}
	}

	/* starting the pcap_loop */	
	pcap_loop(handle, -1, got_packet, NULL);
	return(0);
}
