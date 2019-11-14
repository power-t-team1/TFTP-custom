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
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_IP_ADDRESS 	"127.0.0.1"
#define SERVER_PORT       	5001
#define CLIENT_BUFF_SIZE	20

int main()
{
	int sock_fd,c_size, flag = 0, connected = 0;
	char client_buff[CLIENT_BUFF_SIZE] = {'\0'};
	struct sockaddr_in serv_addr; 
	char *command = NULL;
	char *buffer = NULL;
	char *mode = "netascii";
	char *filename = NULL;
	char *packet = NULL;
	char ack_packet[4]; 
	int len, count, wait = 50000;
	char *error = NULL;
	char send_buffer[513] = {'\0'};
	char data_buffer[517] = {'\0'};
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
	while(1)
	{
		printf("tftp>");
		gets(client_buff);
		if(client_buff[0] != '\0')
		{
			//printf("usages");
			command = strtok(client_buff, " ");
			buffer = strtok(NULL, " ");
			if(strcmp(command,"exit") == 0)
			{
				exit(0);
			}
			if(buffer == NULL)
			{
				printf("enter the correct IP address!\n");
				printf("usages: connect 127.0.0.1\n");
				continue;
			}
			//exit(1);
			break;
		}
	}
	
	
	//Connecting to the desired server(connect loop)
	while(1)
	{
		if(connected)
		{
			break;
		}

		if ((strcmp(command, "connect") == 0) && (flag == 0) )
		{
			connected = 1;
			flag = 1;
			serv_addr.sin_addr.s_addr = inet_addr(buffer);

			/* Send the message to server */
			c_size = sendto(sock_fd, client_buff, strlen(client_buff) + 1, 0,
					(struct sockaddr *)&serv_addr, sizeof(serv_addr));

			if(c_size)
				printf("Message sent to server successsfully, please check\n");
			else
				printf("Error: Message sending failed\n");
			break;
		}
		else
		{
			printf("enter the correct IP address!\n");
			printf("usages: connect 127.0.0.1\n");
			flag = 0;

			//gets again
			while(1)
			{
				printf("tftp>");
				gets(client_buff);
				if(client_buff[0] != '\0')
				{
					printf("usages");
					break;
				}
			}
			command = strtok(client_buff, " ");
			buffer = strtok(NULL, " ");
			
			continue;		
		}
	}

	//Receiving the PORT number from the server
	recvfrom(sock_fd, (void *)client_buff, CLIENT_BUFF_SIZE, 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));
	printf("received port %s\n", client_buff);

	serv_addr.sin_port = htons(atoi(client_buff)); 

	while(1)
	{
		/* Get the message from client */
		//	printf("Enter the message you want to sent to server:\n");
		
		while(1)
		{
			printf("tftp>");
			gets(client_buff);
			if(client_buff[0] != '\0')
			{
				break;
			}
		}
		command = strtok(client_buff, " ");
		
		//Section to handle get command and receive data from server
		if (strcmp(command, "get") == 0)
		{
			filename = strtok(NULL, " ");
			if(filename == NULL)
			{
				printf("USAGES: get <file name>\n\tput <file name>\n\texit or bye: to exit\n");
				continue;
			}
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
				printf("Message sent to server successsfully.\n");
			else
				printf("Error: Message sending failed\n");

			int my_fd, tot = 0;
			if((my_fd = open(filename, O_RDWR | O_CREAT , 00666)) != -1)
			{	
				int my_count = 0;
				while(recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in)) != 0)
				{
					//printf("chunk : %ld\n", strlen(data_buffer+4));
					if (data_buffer[1] == '3')
					{
						tot += strlen(data_buffer+4);
						//printf("Data from server \n%s\n", data_buffer);
						printf("recieved data packet block %d\t",++my_count);
						//Require to write the received data into a file
						write(my_fd,data_buffer+4,strlen(data_buffer+4));	
						//memset(data_buffer+4,'\0',513);

						//acknowledgement	
						ack_packet[0] = '0';
						ack_packet[1] = '4';
						ack_packet[2] = data_buffer[2];
						ack_packet[3] = data_buffer[3];

						if(sendto(sock_fd, ack_packet, strlen(ack_packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))
						{
							printf("Sending Acknowledge packet\n");
						}

						if (strlen(data_buffer+4) < 512)
						{
							close(my_fd);
							break;
						}
						memset(data_buffer+4,'\0',513);
					}
					else if(data_buffer[1] == '5')
					{
						remove(filename);
						printf("Error: %s", data_buffer+4);
						break;
					}
					else
					{
						remove(filename);
					}
						
				}
				
				printf("total recieved %d bytes\n",tot);
				
			}
		}
		
		//Section to handle the put command and write the data to the server
		else if (strcmp(command, "put") == 0)
		{
			int put_count = 0;
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
			while ((fread(send_buffer, 1, 512, fd)) != 0)
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
				//printf("Packet: %s\n", packet);

				sendto(sock_fd, packet, strlen(packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
				memset(send_buffer,'\0',513);
				printf("Sending Data packet %d\t", ++put_count);				
				if(recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in)))
				{
					printf("Received acknowledgement\n");
				}
				
				while(data_buffer[1] != '4')
				{
					while(wait)
					{
						wait--;
					}
					wait = 50000;

					sendto(sock_fd, packet, strlen(packet) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
					printf("Sending Data packet %d\t", put_count);				
					recvfrom(sock_fd, (void *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(struct sockaddr_in));
					printf("Received acknowledgement\n");
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
				printf("exiting TFTP client\n");
			else
				printf("Error: Message sending failed\n");

			close(sock_fd);
			exit(0);
		}
		else
		{
			printf("USAGES: get <file name>\n\tput <file name>\n\texit or bye: to exit\n");
			continue;
		}
		

	}
}
