/*****************************/
/*Titel: Assignment4_Client.c*/
/*   Author: Khanjan Desai   */
/*   Date  : Feb 11, 2016    */
/* Client program to execute */
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

		//Write data if run using PIPE with rx
		if (write(STDOUT_FILENO,buf,BUFSIZE) < 0)
		{
			perror(": Write failed\n");
			exit(0);
		}
	}

	// Relese Memory
	if(record_tx)
	{
		free(record_tx);
	}
}




