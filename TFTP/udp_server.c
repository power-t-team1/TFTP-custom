//#include "common.h"
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
	struct sockaddr_in cli_addr; 
	char serv_buffer[SERVER_BUFF];
	char child_port[8] = {'\0'};
	int pid, child_port_number = 20000;
	char *filename = NULL;
	FILE *fd = NULL;
	char buffer[512] = {'\0'};
	char *packet = NULL;
	short count;

	/* Create a UDP socket */
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* Bind it to a particular IP address & port) */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT); 

	bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	printf("Emertxe: Server is waiting...\n");



	while(1)
	{
		/* Try to get some actual data from client */
		cli_len = sizeof(struct sockaddr_in);
		//int bytes;
		recvfrom(sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);
		if (strncmp(serv_buffer,"connect", 7) == 0)
		{
			//increment port number
			child_port_number++;

			sprintf(child_port, "%d", child_port_number);

			sendto(sock_fd, child_port, strlen(child_port) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
			//we will fork
			pid = fork();

			if(pid == 0)
			{
				int child_sock_fd;

				/* Create a UDP socket */
				child_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

				/* Bind it to a particular IP address & port) */
				serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
				serv_addr.sin_port = htons(child_port_number); 

				bind(child_sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
				while(1)
				{
					recvfrom(child_sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);
					/* Print the data received from client */
					printf("Here is the client data: %s\n",serv_buffer);

					printf("Sender Info:\n");
					printf("Sender IP: %s\n", inet_ntoa(cli_addr.sin_addr));
					printf("Sender Port: %d\n", ntohs(cli_addr.sin_port));
					
					if (serv_buffer[1] == '1')
					{
						filename = strtok(&serv_buffer[2], "0");
						printf("Filename: %s\n", filename);

						fd =  fopen(filename, "r");
						if (fd == NULL)
						{
							perror("fopen");
						}
						
						packet = malloc(4);
						packet[0] = '0';
						packet[1] = '3';
						count = 0;
						while (fread(buffer, 1, 512, fd))
						{
							count++;
							if (count < 10)
							{
								packet[2] = '0';
								sprintf(&packet[3], "%d", count);
							}
							else
							{
								sprintf(&packet[2], "%d",(count / 10));
								sprintf(&packet[3], "%d",(count % 10));
							}
							packet = realloc(packet, 516);
							strcat(packet, buffer);
							printf("Packet: %s\n", packet);

							sendto(child_sock_fd, packet, strlen(packet) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

							printf("Message sent\n");

						}

					}
					else if (serv_buffer[1] == '2')
					{
						filename = strtok(&serv_buffer[2], "0");
						printf("Filename: %s\n", filename);
					}
					else if (!(strcmp(serv_buffer, "exit")) || !(strcmp(serv_buffer, "bye")))
					{
						close(child_sock_fd);
						exit(0);

					}
					

				}
			}
		}
		//in child process
		/*
		   bind to the new port
		   and communicate

		 */

		// parent process should be in loop keeps on running



		/* Close the socket now */
		//	close(sock_fd);
	}
}
