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
#define BUFSIZE 1024

int main(void)
{	
	//create Spec that defines samplerate, channels and format
	const pa_sample_spec spec = {
		.format = PA_SAMPLE_S16LE,
        	.rate = 44100,
        	.channels = 2
	};
	
	pa_simple *rx_play = NULL;
	int error;
	
	// Create new stream to play
	if((rx_play = pa_simple_new(NULL,"VOIP_rx.c",PA_STREAM_PLAYBACK,NULL,"PLAYBACK",&spec,NULL,NULL,&error)) == NULL)
	{
		printf("pa_simple_new() failed: %s\n",pa_strerror(error));
		exit(0);
	}

	unsigned char buf[BUFSIZE];
	int r=0;
	while(1)
	{
		//Read data if run using PIPE with tx
		if(((r = read(STDIN_FILENO, buf, BUFSIZE))) < 0)
		{
			perror(":Read failed\n");
			exit(0);
		}
			
		// Play data
		if(pa_simple_write(rx_play,buf,BUFSIZE,&error) < 0)
		{
			printf("pa_simple_right() failed: %s\n",pa_strerror(error));
			exit(0);		
		}
	}
	
	// Play till end
	if(pa_simple_drain(rx_play, &error) < 0)
	{
		printf("pa_simple_drain() failed: %s\n", pa_strerror(error));
		exit(0);
	}
	
	// Free the space
	if (rx_play)
        {
		pa_simple_free(rx_play);
	}	
}


