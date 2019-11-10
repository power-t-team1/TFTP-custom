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
    gets(client_buff);

    /* Send the message to server */
    c_size = sendto(sock_fd, client_buff, strlen(client_buff) + 1, 0,
	    (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(c_size)
	printf("Message sent to server successsfully, please check\n");
    else
	printf("Error: Message sending failed\n");

	recvfrom(sock_fd, (void *)client_buff, CLIENT_BUFF_SIZE, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("received port %s\n", client_buff);


    close(sock_fd);
}
