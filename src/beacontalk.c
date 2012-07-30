#include <stdio.h>
#include <string.h>

#include "beaconpacket.h"

#define SCREEN_HEIGHT 100

int i, j;
char nick[10], message[500], concatenated[512], text[100][512];
char *dev;

void shiftarray(){
	for (i = SCREEN_HEIGHT; i > 0; i--){
		printf("\n");
	}
	if (text[0][0] != 0 && text[1][0] != 0 && text[100][0] != 0){
		text[100][0] = 0;
	}
	for (i = 98; i >= 0; i--){
		if (text[i][0] != 0 && text[i+1][0] == 0){
			for (j = 0; j < 512; j++){
				text[i+1][j] = text[i][j];
			}
			text[i][0] = 0;
		}
	}
	for (i = 99; i > 0; i--){
		if (text[i][0] != 0){
			printf("%s", text[i]);
		}
	}
	fputs("enter message: ", stdout);
	fflush(stdout);
}

void printmessage(){

	shiftarray();
	fgets(message, sizeof message, stdin);
	strcpy(concatenated, nick);
	strcat(concatenated, ": ");
	strcat(concatenated, message);
	/*broadcast the message here */
	send_beaconpacket(concatenated);
}	

int main(int argc, char *argv[]){

	/* defines the wireless device that will be used */
	if (argc < 2){
	printf("enter wireless interface as argument\n");
		return(0);
	}
	dev = argv[1];

	/* defines the nickname that will be send with your messages */
	fputs("nick (max 10 char): ", stdout);
	fflush(stdout);
	fgets(nick, 10, stdin);	
	for (i = 0; i < sizeof(nick); i++){
		if (nick[i] == '\n'){
			nick[i] = '\0';
		}
	}
	
	/* makes sure no random garbage is printed */
	for (i = 99; i >= 0; i--){
		text[i][0] = 0;
	}

	/* this function is a callback for the threads in threads.c */
	void threads(int thread_choice){
		switch (thread_choice){
			case 1:
				/* listens for incoming messages */
				get_beaconpacket();
				break;

			case 2:
				/* takes care of the sending and displaying of messages */
				while (1){	
					printmessage();
				}
				break;

			default:
				printf("ERROR: non valid input. Check the (*f)(x) in threads.c\n");
		}
	}

	/* initiates the threading */
	thread_init(threads);

}

