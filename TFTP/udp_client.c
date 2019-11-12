/*------------------------------------------------------------------------------ 
 *
 *   Author         : Emertxe (http://www.emertxe.com) 
 *   Date           : Tue 01 Mar 2016 16:18:10 IST
 *   File           : t022.c
 *   Title          : IPC mechanism - Usage of UDP socket - Client 
 *   Description    : Sockets offer interfaces with TCP/IP stack inside Linux 
 *                    Kernel, using which remote connections can be established
 *                    over a network. This program demonstrates various steps
 *                    to establish successful connection using UDP sockets as
 *                    a connection-less client. 
 *
 *----------------------------------------------------------------------------*/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define SERVER_IP_ADDRESS 	"127.0.0.1"
#define SERVER_PORT       	5001
#define CLIENT_BUFF_SIZE	20

int main()
{
	int sock_fd,c_size;
	char client_buff[CLIENT_BUFF_SIZE];
	struct sockaddr_in serv_addr; 
	char *command = NULL;
	char *buffer = NULL;
	char *mode = "netascii";
	char *filename = NULL;
	char *packet = NULL;
	char ack_packet[4]; 
	int len, count, wait = 50000;
	char *error = NULL;
	char send_buffer[512] = {'\0'};
	char data_buffer[516];
	FILE *fd = NULL;

	/* Create a client socket */
	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Error: Could not create socket\n");
		return 1;
	} 

	/* Binding with client socket is optional here */

	/* Populate it with server's address details */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT); 

	/* Get the message from client */
	printf("Enter the message you want to sent to server:\n");
	gets(client_buff);

	command = strtok(client_buff, " ");
	buffer = strtok(NULL, " ");
	
	//Connecting to the desired server
	if (strcmp(command, "connect") == 0)
	{
		serv_addr.sin_addr.s_addr = inet_addr(buffer);

		/* Send the message to server */
		c_size = sendto(sock_fd, client_buff, strlen(client_buff) + 1, 0,
				(struct sockaddr *)&serv_addr, sizeof(serv_addr));

		if(c_size)
			printf("Message sent to server successsfully, please check\n");
		else
			printf("Error: Message sending failed\n");
	}

	//Receiving the PORT number from the server
	recvfrom(sock_fd, (void *)client_buff, CLIENT_BUFF_SIZE, 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));
	printf("received port %s\n", client_buff);

	serv_addr.sin_port = htons(atoi(client_buff)); 

	while(1)
	{
		/* Get the message from client */
		//	printf("Enter the message you want to sent to server:\n");
		gets(client_buff);
		command = strtok(client_buff, " ");
		
		//Section to handle get command and receive data from server
		if (strcmp(command, "get") == 0)
		{
			filename = strtok(NULL, " ");
			packet = malloc(2);
			packet[0] = '0';
			packet[1] = '1';

			packet = realloc(packet, (strlen(filename) + strlen(mode) + 5));
			strcat(packet, filename);
			strcat(packet, "0");
			strcat(packet, mode);
			strcat(packet, "0");

			printf("Filename: %s\nMode: %s\n", filename, mode);

			/* Send the message to server */
			c_size = sendto(sock_fd, packet, strlen(packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

			if(c_size)
				printf("Message sent to server successsfully, please check\n");
			else
				printf("Error: Message sending failed\n");

			while(len = recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in)))
			{
				if (data_buffer[1] == '3')
				{
					printf("Data from server \n%s", data_buffer);

					//Require to write the received data into a file

					ack_packet[0] = '0';
					ack_packet[1] = '4';
					ack_packet[2] = data_buffer[2];
					ack_packet[3] = data_buffer[3];

					sendto(sock_fd, ack_packet, strlen(ack_packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

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
		
		//Section to handle the put command and write the data to the server
		else if (strcmp(command, "put") == 0)
		{
			filename = strtok(NULL, " ");
			packet = malloc(2);
			packet[0] = '0';
			packet[1] = '2';

			packet = realloc(packet, (strlen(filename) + strlen(mode) + 5));
			strcat(packet, filename);
			strcat(packet, "0");
			strcat(packet, mode);
			strcat(packet, "0");

			printf("Filename: %s\nMode: %s\n", filename, mode);

			/* Send the message to server */
			c_size = sendto(sock_fd, packet, strlen(packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

			if(c_size)
				printf("Message sent to server successsfully, please check\n");
			else
				printf("Error: Message sending failed\n");

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

				sendto(sock_fd, error, strlen(error) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
				continue;
			}

			packet = malloc(4);
			packet[0] = '0';
			packet[1] = '3';
			count = 0;
			while (fread(send_buffer, 1, 512, fd))
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
				strcat(packet, send_buffer);
				printf("Packet: %s\n", packet);

				sendto(sock_fd, packet, strlen(packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

				recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));

				while(data_buffer[1] != '4')
				{
					while(wait)
					{
						wait--;
					}
					wait = 50000;

					sendto(sock_fd, packet, strlen(packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

					recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));

					if (data_buffer[1] != '4')
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
		
		//Section to handle the Exit command on the client side
		else if((strcmp(command, "exit") == 0) || (strcmp(command, "bye") == 0))
		{

			/* Send the message to server */
			c_size = sendto(sock_fd, command, strlen(command) + 1, 0,
					(struct sockaddr *)&serv_addr, sizeof(serv_addr));

			if(c_size)
				printf("Message sent to server successsfully, please check\n");
			else
				printf("Error: Message sending failed\n");

			close(sock_fd);
			exit(0);
		}

	}
}
