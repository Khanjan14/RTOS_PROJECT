/*****************************/
/*   Titel: VOIP_rx.c        */
/*   Author: Khanjan Desai   */
/*   Date  : March 4,2016    */
/* VOIP server Transmit voice*/
/*Client-Server using Socket */
/*****************************/

// Compile : gcc filename.c -lpulse -lpulse-simple -o filename //
// Run	   : ./filename <IP> <PORT>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#define BUFSIZE 1024

int sockfd = 0, n,i;


void handler(int signo)
{
    	if(signo = SIGINT)
    	{
		printf("Shut down");
		int ret = close(sockfd);
		sleep(1);
		if(ret == -1)	
		{	
			printf("Closing Error");
		}
		exit(1);
	}
}



int main(int argc, char *argv[])
{

	// Read IP and port
    	char * ip = argv[1];
    	int ip1 = atoi(argv[2]);	
    	struct sockaddr_in serv_addr; 

	// Signal Handler
    	if(signal(SIGINT,handler) == SIG_ERR)
    	{
		printf("Cant recieve signal");
    	}    


	
        // Error check for sockfd
    	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
	{
		perror("sockfd error:");
	}
    	memset(&serv_addr, '0', sizeof(serv_addr)); 

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = inet_addr(ip);	// Give IP
    	serv_addr.sin_port = htons(ip1); 		// Give Port


     
    	int con;
	//Error check for Connect    
    	if((con  = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    	{	
       		perror("Error in connect call :");
       		return 1;
    	}

	//create Spec that defines samplerate, channels and format
	const pa_sample_spec spec = {
		.format = PA_SAMPLE_S16LE,
        	.rate = 44100,
        	.channels = 2
	};
	
	pa_simple *record_tx = NULL;
	int error;

	// Create new stream to play
	if ((record_tx = pa_simple_new(NULL,"VOIP_tx.c",PA_STREAM_RECORD,NULL,"record",&spec,NULL,NULL,&error))== NULL)
	{
		perror("ERROR connection:");
		exit(0);
	}

	unsigned char buf[BUFSIZE];
		
	while(1)
	{
		// Record audio
		if (pa_simple_read(record_tx,buf,BUFSIZE,&error) < 0)
		{
			printf("pa_simple_read() failes: %s\n",pa_strerror(error));
			exit(0);
		}

	//send if using socket
		if(send(sockfd,buf,BUFSIZE,0) < 0)
		{
			perror("Send failed\n");
			exit(0);
		}
	//Write data if run using PIPE with rx
/*		if (write(STDOUT_FILENO,buf,BUFSIZE) < 0)
		{
			perror(": Write failed\n");
			exit(0);
		}*/
	}

	// Relese Memory
	if(record_tx)
	{
		free(record_tx);
	}
}



