#include <stdio.h>
#include <string.h>
#include <pcap.h>

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
	char errbuf[512];

	/* defines other stuff */
	u_char buf[1024];
	char message[850], messagelen[6];
	char type[] = "\x0\x0\xd\x0\x4\x80\x2\x0\x2\x0\x0\x0\x0\x80\x0\x0\x0\xff\xff\xff\xff\xff\xff\xf8\xb7\x6f\x7b\xc2\x59\xf8\xb7\x6f\x7b\xc2\x59\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x64\x0\x11\x0\x0\x5\x73\x74\x6f\x6e\x65\x1\x4\x82\x84\x8b\x96\x3\x1\xc\x4\x6\x1\x2\x0\x0\x0\x0\x5\x4\x0\x1\x0\x0\xdd\x18\x0\x50\xf2\x1\x1\x0\x0\x50\xf2\x4\x1\x0\x0\x50\xf2\x4\x1\x0\x0\x50\xf2\x2\x0\x0"; /* stone beacon frame header */
	char size[] = "size", end[] = "end";
	
	printf("enter message to send:\n");
	while (1){
		memset(message, 0, sizeof(message));
		/* gets user input */
		fgets(message, sizeof message, stdin);
		if (message[0] == '\0')
			return(0);

		/* opening pcap session */
		handle = pcap_open_live(dev, BUFSIZ, 0, 1024, errbuf);
		if (handle == NULL){
			printf("Couldn't open device %s\n", errbuf);
			return(1);
		}

		/* adds the beacon frame */ 
		memcpy(buf, type, sizeof(type));
		sprintf(messagelen, "%.5d", strlen(message));
		
		/* adds the size marker */
		memcpy(buf+sizeof(type), size, strlen(size));

		/*adds the size number */
		memcpy(buf+sizeof(type)+strlen(size), messagelen, 5);

		/* adds the message */
		memcpy(buf+sizeof(type)+strlen(size)+sizeof(messagelen), message, strlen(message));
		
		/* ends the stone data string */	
		memcpy(buf+sizeof(type)+strlen(size)+sizeof(messagelen)+strlen(message), end, strlen(end));

		if ( -1 == pcap_sendpacket(handle, buf, 1024))
			printf("sending package failed\n");
	}
	return(0);
}
