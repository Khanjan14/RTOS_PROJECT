/*****************************/
/*   Titel: VOIP_rx.c        */
/*   Author: Khanjan Desai   */
/*   Date  : March 4,2016    */
/* VOIP server recieve voice */
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

int listenfd = 0, connfd = 0;


    
// Signal Handler

void handler(int signo)
{
    	if(signo = SIGINT)
    	{
		printf("Shut down");
		int ret = close(listenfd);
		sleep(1);
		if(ret == -1)	printf("Closing Error");
		exit(0);
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
    	if(signal(SIGINT,handler) == SIG_ERR)
    	{
		printf("Cant recieve signal");
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
	pa_simple *rx_play = NULL;
	int error; 
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
	

		
	
	
	
	while(1)
	{

		unsigned char buf[BUFSIZE];
		int r=0;


		//Read data if run using PIPE with tx
/*		if(((r = read(STDIN_FILENO, buf, BUFSIZE))) < 0)
		{
			perror(":Read failed\n");
			exit(0);
		}
*/
		//Read Recieved data
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
				// Play data
		if(pa_simple_write(rx_play,buf,BUFSIZE,&error) < 0)
		{
			printf("pa_simple_right() failed: %s\n",pa_strerror(error));
			exit(0);		
		}

	}
	
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
	close(connfd);			
}

