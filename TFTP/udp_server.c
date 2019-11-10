/*------------------------------------------------------------------------------ 
 *
 *   Author         : Emertxe (http://www.emertxe.com) 
 *   Date           : Tue 01 Mar 2016 16:18:10 IST
 *   File           : t022.c
 *   Title          : IPC mechanism - Usage of UDP socket - Server 
 *   Description    : Sockets offer interfaces with TCP/IP stack inside Linux 
 *                    Kernel, using which remote connections can be established
 *                    over a network. This program demonstrates various steps
 *                    to establish successful connection using UDP sockets as
 *                    a connection-less server. 
 *
 *----------------------------------------------------------------------------*/
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP  	"127.0.0.1"
#define SERVER_PORT 	5001
#define SERVER_LENGTH 	10
#define SERVER_BUFF	20

int main()
{
	int sock_fd,buffer_len,cli_len;
	struct sockaddr_in serv_addr; 
	struct sockaddr_in    cli_addr; 
	char serv_buffer[SERVER_BUFF];

	printf("Emertxe: Server is waiting...\n");

	/* Create a UDP socket */
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* Bind it to a particular IP address & port) */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT); 

	bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
	int pid, pno = 20000;


	/* Try to get some actual data from client */
	cli_len = sizeof(struct sockaddr_in);
	int bytes;
	while(1)
	{
		if((bytes = recvfrom(sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len)) != 0)
		{

			//increment port number
			pno++;
			//we will fork
			pid = fork();


			if(pid == 0)
			{
				char k[10];
				sprintf(k, "%d", pno);
				sendto(sock_fd, k, strlen(k) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
				close(sock_fd);
				int ch_sfd;

				/* Create a UDP socket */
				ch_sfd = socket(AF_INET, SOCK_DGRAM, 0);

				/* Bind it to a particular IP address & port) */
				serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
				serv_addr.sin_port = htons(pno); 

				bind(ch_sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

			}
		}
		//in child process
		/*
		   bind to the new port
		   and communicate

		 */

		// parent process should be in loop keeps on running


		/* Print the data received from client */
		printf("Here is the client data: %s\n",serv_buffer);

		printf("Sender Info:\n");
		printf("Sender IP: %s\n", inet_ntoa(cli_addr.sin_addr));
		printf("Sender Port: %d\n", ntohs(cli_addr.sin_port));

		/* Close the socket now */
		close(sock_fd);
	}
}
