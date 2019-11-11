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
	int sock_fd,c_size;
	char client_buff[CLIENT_BUFF_SIZE];
	struct sockaddr_in serv_addr; 

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
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

	/* Get the message from client */
	printf("Enter the message you want to sent to server:\n");

	/* Send the message to server */
	//    while(1)
	//	{
	gets(client_buff);
	c_size = sendto(sock_fd, client_buff, strlen(client_buff) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if(c_size)
		printf("Message sent to server successsfully, please check\n");
	else
		printf("Error: Message sending failed\n");
	//	}
	recvfrom(sock_fd, (void *)client_buff, CLIENT_BUFF_SIZE, 0, (struct sockaddr *) &serv_addr,(socklen_t *) sizeof(serv_addr));
	printf("received port %s\n", client_buff);


	close(sock_fd);


	char temp[50];
	//Connect with the new port address
	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Error: Could not create socket\n");
		return 1;
	} 

	/* Binding with client socket is optional here */
	printf("port number as integer %d\n", atoi(client_buff));
	/* Populate it with server's address details */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(client_buff)); 
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

	/* Get the message from client */
	printf("Enter the message you want to sent to server:\n");

	//Create packets
	char *rwpack = (char *)malloc(2);
	char *ackpack = (char *)malloc(4);
	char *dpack = (char *)malloc(4);
	char *erpack = (char *)malloc(4);
	while(1)
	{
		gets(client_buff);
   c_size = sendto(sock_fd, client_buff, strlen(client_buff) + 1, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

		char *cmd = NULL, *name = NULL;
		char *file_name = NULL;
		//Tftp starts from here
		strcpy(temp, client_buff);
		cmd = strtok(temp, " ");
		name = strtok(NULL, " ");
		strcpy(file_name, name);
		strcat(name, " netascii");
		if(strcmp(cmd, "get") == 0)
		{
			char *buff = malloc(513 * sizeof(char));

			//Create the read request packet
			rwpack[0] = '0';
			rwpack[1] = '0';
			rwpack = realloc(rwpack, 3+strlen(name));
			strcpy((rwpack+2), name);
			printf("%s\n", rwpack);
			//Send to server a request packet
			c_size = sendto(sock_fd, rwpack, strlen(rwpack) + 1, 0,(struct sockaddr *)&serv_addr, sizeof(serv_addr));


			int fd;
			if((fd = open(file_name, O_RDWR | O_CREAT | O_EXCL | O_TRUNC , 00777)) != -1);
			{
				while(recvfrom(sock_fd, (void *)buff, 513, 0, (struct sockaddr *) &serv_addr,(socklen_t *) sizeof(serv_addr)) > 0)
				{					
					write(fd,buff,513);
				}
			}
			close(fd);
		
			

		}
		if(strcmp(cmd, "put") == 0)
		{
		}
		if(strcmp(cmd, "quit") == 0)
		{
			exit(0);
		}


 

		



}	


}
