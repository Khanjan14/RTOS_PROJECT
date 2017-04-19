/*****************************/
/* Titel: VOIP_Timer_ser.c   */
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
#define BUFSIZE 4096

int listenfd = 0, connfd = 0;
pa_simple *rx_play = NULL;
int error; 
	


static jmp_buf buf;
void signal_handler (int sig)
{
	int16_t buf[BUFSIZE],buf1[BUFSIZE];
	int16_t r=0;

	if(sig == SIGINT)
	{
		printf("Shut down");
		int ret = close(listenfd);
		sleep(1);
		if(ret == -1)	printf("Closing Error");
		exit(0);
	}
	if(sig ==SIGALRM)
	{
		r = recv(connfd,buf,BUFSIZE,0);
		if(r <= 0)
		{
			perror("Rx error\n");
    	  	//System reconnect		
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    			if (connfd==-1)
			{
				perror("Error:");
				exit(1);
			}   
			
		}
		for(int i =0; i<BUFSIZE;i++)
		{
			buf1[i] = alaw2linear(buf[i]);
		}
		// Play data
		if(pa_simple_write(rx_play,buf1,BUFSIZE,&error) < 0)
		{
			printf("pa_simple_right() failed: %s\n",pa_strerror(error));
			exit(1);		
		}
	}
}


int main(int argc, char *argv[])
{		
	// Read IP and Port address
    char * ip = argv[1];
    int ip2= atoi(argv[2]);

   	struct sockaddr_in serv_addr; 
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
   	memset(&serv_addr, '0', sizeof(serv_addr));
   	serv_addr.sin_family = AF_INET;
   	serv_addr.sin_addr.s_addr = inet_addr(ip);
   	serv_addr.sin_port = htons(ip2);
	
	// Signal Handler 
  	if(signal(SIGINT,signal_handler) == SIG_ERR)
    {
		printf("Cant recieve signal");
   	}
   	// Signal Handler for Timer
	if(signal(SIGALRM,signal_handler)==SIG_ERR)
	{
		perror("Error:");
	}
	// Error handler for bind
   	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
	{
    		perror("Error in binding");
	    	return 1;
	} 
	// Error Handler for listenfd
   	if(listen(listenfd, 10)<0)
   	{
   		perror("Error in listening");
   		return 1;
   	}
	// If all correct start server
    	printf("Waiting for call...\n");
	//create Spec that defines samplerate, channels and format
	const pa_sample_spec spec = {
		.format = PA_SAMPLE_S16LE,
        	.rate = 44100,
        	.channels = 2
	};

	// Error handling for recieving Client IP
    	
    	if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL))<0)
	{
		perror("Connect error:");   
		exit(0);
	}
        

	// Create new stream to play
	if((rx_play = pa_simple_new(NULL,"VOIP_rx.c",PA_STREAM_PLAYBACK,NULL,"PLAYBACK",&spec,NULL,NULL,&error)) == NULL)
	{
		printf("pa_simple_new() failed: %s\n",pa_strerror(error));
		exit(0);
	}

		
    ualarm (6000, 6000);
    
	
	while(1)
		pause();
	
	/*// Play till end
	if(pa_simple_drain(rx_play, &error) < 0)
	{
		printf("pa_simple_drain() failed: %s\n", pa_strerror(error));
		exit(0);
	}*/
	
	// Free the space
	if (rx_play)
        {
		pa_simple_free(rx_play);	
	}	
//	close(connfd);			
}

