/*****************************/
/* Titel: VOIP_Timer_cli.c   */
/*   Author: Khanjan Desai   */
/*   Date  : March 4,2016    */
/* VOIP server recieve voice */
/*Client-Server using Socket */
/*****************************/


// Compile : gcc filename.c -lpulse -lpulse-simple -o filename //
// Run	   : ./filename <IP> <PORT>


#include <setjmp.h>
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
#include "G711.c"
#define BUFSIZE 1024

int sockfd = 0, n,i;
pa_simple *record_tx = NULL;
int error; 
struct sockaddr_in serv_addr; 

int buf[BUFSIZE],buf1[BUFSIZE];

void signal_handler (int sig)
{
	//printf("OK\n");
	if(sig == SIGINT)
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
	if(sig == SIGALRM)
	{
		// Record audio
		if (pa_simple_read(record_tx,buf,BUFSIZE,&error) < 0)
		{
			printf("pa_simple_read() failes: %s\n",pa_strerror(error));
			exit(0);
		}
		for(int i=0;i<BUFSIZE;i++)
		{
			buf1[i] = linear2ulaw(buf[i]);
		}
		//send if using socket
		if(send(sockfd,buf,BUFSIZE,0) < 0)
		{
			perror("Send failed\n");
			exit(0);
		}
	}
}

int main (int argc, char ** argv)
{
	// Read IP and port
    char * ip = argv[1];
    int ip1 = atoi(argv[2]);	

    // Signal Handler for Timer
    if(signal(SIGALRM,signal_handler)==SIG_ERR)
    {
    	perror("Error:");
    	exit(1);
    }
	//Signal Handler for ctrl+c
    if(signal(SIGINT,signal_handler)==SIG_ERR)
    {
    	perror("Error:");
    	exit(1);
    }
	

        // Error check for sockfd
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
	{
		perror("sockfd error:");
		exit(1);
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
	

	// Create new stream to play
	if ((record_tx = pa_simple_new(NULL,"test3_cli.c",PA_STREAM_RECORD,NULL,"record",&spec,NULL,NULL,&error))== NULL)
	{
		perror("ERROR connection:");
		exit(0);
	}

	// Starting clock at 10 us and repeat every 10us.
    ualarm (6000, 6000);
    while(1)
    	pause();
    	

	// Relese Memory
	if(record_tx)
	{
		free(record_tx);
	}


    close(sockfd);
    pa_simple_free(record_tx);


    return 0;
}	


