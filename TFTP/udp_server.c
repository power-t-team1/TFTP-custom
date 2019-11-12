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
	int pid, wait = 50000, child_port_number = 20000;
	char *filename = NULL;
	char *error = NULL;
	FILE *fd = NULL;
	char buffer[512] = {'\0'};
	char *packet = NULL;
	char ack_packet[4];
	int len;
	char data_buffer[516];
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
					
					//Section to handle get command from client
					if (serv_buffer[1] == '1')
					{
						filename = strtok(&serv_buffer[2], "0");
						printf("Filename: %s\n", filename);

						fd =  fopen(filename, "r");
						if (fd == NULL)
						{
							perror(filename);
							error = malloc(4);
							error[0] = '0';
							error[1] = '5';
							error[2] = '0';
							error[3] = '1';
							error = realloc (error, (strlen(filename) + 16));
							strcat(error, filename);
							strcat(error, " not found0\n");

							sendto(child_sock_fd, error, strlen(error) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
							continue;
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

							recvfrom(child_sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);

							while(serv_buffer[1] != '4')
							{	
								while(wait)
								{
									wait--;
								}
								wait = 50000;

								sendto(child_sock_fd, packet, strlen(packet) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

								recvfrom(child_sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);

								if (serv_buffer[1] != '4')
								{
									continue;
								}
								else
								{
									break;
								}
							}
							printf("Message sent\n");

						}

					}

					//Section to handle put command from client
					else if (serv_buffer[1] == '2')
					{
						filename = strtok(&serv_buffer[2], "0");
						printf("Filename: %s\n", filename);
						while(len = recvfrom(child_sock_fd,(void *)data_buffer,sizeof(data_buffer),0,(struct sockaddr *) &cli_addr, &cli_len))
						{
							if (data_buffer[1] == '3')
							{
								printf("Data from client \n%s", data_buffer);

								//Require to write the received data into a file

								ack_packet[0] = '0';
								ack_packet[1] = '4';
								ack_packet[2] = data_buffer[2];
								ack_packet[3] = data_buffer[3];

								sendto(child_sock_fd, ack_packet, strlen(ack_packet) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

								if (len < 512)
								{
									break;
								}
							}
							else if(data_buffer[1] == '5')
							{
								printf("Error: %s", data_buffer);
								break;
							}
						}
					}

					//Section to handle Exit call from the client
					else if (!(strcmp(serv_buffer, "exit")) || !(strcmp(serv_buffer, "bye")))
					{
						close(child_sock_fd);
						exit(0);

					}


				}
			}
		}
	}
}
