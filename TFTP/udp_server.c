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
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

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
	ssize_t num;
	printf("Emertxe: Server is waiting...\n");

	/* Create a UDP socket */
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* Bind it to a particular IP address & port) */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT); 

	bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
	int pid, pno = 20001;


	/* Try to get some actual data from client */
	cli_len = sizeof(struct sockaddr_in);
	int bytes;


	while(1)
	{
		//Receive first message from client
		num = recvfrom(sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);
		if(num > 0)
		{
			pno++;
			char k[10];
			{

				//increment port number
				//we will fork
				pid = fork();


				if(pid == 0)
				{
					char k[10], temp[50];
					char *cmd = NULL, *name = NULL;
					sprintf(k, "%d", pno);
					sendto(sock_fd, k, strlen(k) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
					close(sock_fd);
					int ch_sfd;
					char fname[20], *temp1 = NULL ;
					char *saveptr, *saveptr1;

					//Create packets
					char *rwpack = (char *)malloc(2);
					char *ackpack = (char *)malloc(4);
					char *dpack = (char *)malloc(4);
					char *erpack = (char *)malloc(4);


					//Create new sock fd with new port number
					ch_sfd = socket(AF_INET, SOCK_DGRAM, 0);

					serv_addr.sin_family = AF_INET;
					serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
					serv_addr.sin_port = htons(pno); 

					bind(ch_sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
					while(1)
					{
						recvfrom(sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);
						printf("received via %d new port %s\n", pno, serv_buffer);


						//Tftp starts from here
						strcpy(temp, serv_buffer);
						printf("%s\n", temp);
						cmd = strtok_r(temp, " ", &saveptr);
						printf("cmd : %s\n", cmd);
						name = strtok_r(NULL, " ", &saveptr);
						printf("name : %s\n", name);
						
						if(cmd[1] == '0')
						{
							char tmp[50] = {'\0'};
							strcat(tmp,&cmd[2]);
							printf("file name : %s\n", tmp);
							
							printf("we received a get cmd\n");
						//	recvfrom(sock_fd,(void *)serv_buffer,SERVER_BUFF,0,(struct sockaddr *) &cli_addr, &cli_len);
							printf("received the RRQ req via  %d new port %s\n", pno, serv_buffer);
							//strcpy(temp, serv_buffer);
							//printf("temp : %s\n", serv_buffer);
							//temp1 = strtok_r(temp, " ", &saveptr1);
							
							strcpy(fname, tmp);
							printf("fname: %s\n", fname);
							int fd;
							char *buff = malloc(513 * sizeof(char));
							if((fd = open(fname, O_RDWR | O_EXCL | O_TRUNC , 00777) != -1))
							{
								while(read(fd, buff, 513) > 0)
								{
									sendto(sock_fd, buff, strlen(buff) + 1, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));	
								}
								
							}
							close(fd);

						}
						if(strcmp(cmd, "put") == 0)
						{
							printf("we received a put cmd\n");
						}
						if(strcmp(cmd, "quit") == 0)
						{
							//							exit(0);
						}

					}
				}
			}
		}

	}
	close(sock_fd);
}
