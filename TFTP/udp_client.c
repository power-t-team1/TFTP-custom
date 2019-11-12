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
	int len;
	char data_buffer[517];


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




	recvfrom(sock_fd, (void *)client_buff, CLIENT_BUFF_SIZE, 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));
	printf("received port %s\n", client_buff);
	

	serv_addr.sin_port = htons(atoi(client_buff)); 

	while(1)
	{
		/* Get the message from client */
		//	printf("Enter the message you want to sent to server:\n");
		gets(client_buff);
		command = strtok(client_buff, " ");

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

			recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));
			
				printf("Data from server \n%s", data_buffer);
		
		}

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
			c_size = sendto(sock_fd, packet, strlen(packet) + 1, 0,
					(struct sockaddr *)&serv_addr, sizeof(serv_addr));

			if(c_size)
				printf("Message sent to server successsfully, please check\n");
			else
				printf("Error: Message sending failed\n");

		}

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
		}







	}



}
