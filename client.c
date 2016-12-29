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
#include <fcntl.h>

#define deleteCmd "delete"
#define renameCmd "rename"
#define uploadCmd "upload"
#define downldCmd "downld"
#define fileNotFound "File not found\n"


int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0, filefd = 0, rt = 0;
	char recvBuff[256], sendBuff[256], sendBuff1[256], buffer[1024], fileMessage[256];
	char *file_path = "xyz.avi";
	ssize_t read_return;
	struct sockaddr_in serv_addr; 

	if(argc != 4)
	{
		printf("\n Usage: %s <ip of server> <command-name> <filename> \n",argv[0]);
		return 1;
	} 

	memset(recvBuff, '0',sizeof(recvBuff));
	memset(sendBuff, '0',sizeof(recvBuff));
	memset(sendBuff1, '0',sizeof(recvBuff));
        strcpy(sendBuff, argv[2]);
	strcpy(sendBuff1, argv[3]);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080); 

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	} 

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Connect Failed \n");
		return 1;
	} 

        // send command to server
        send(sockfd, sendBuff, sizeof(sendBuff), MSG_WAITALL);
	recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
        printf("Message from the server: %s\n", recvBuff);

	send(sockfd, sendBuff1, sizeof(sendBuff1), MSG_WAITALL);
	recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
        printf("Message from the server: %s\n", recvBuff);
	if (strcmp(argv[2],deleteCmd) == 0) 
	{
		recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
		printf("Message from the server:%s\n", recvBuff);
	}
	else if (strcmp(argv[2],renameCmd) == 0)
	{
		recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
		printf("\nMessage from the server: %s :", recvBuff);
		gets(sendBuff);
		send(sockfd, sendBuff, sizeof(sendBuff), MSG_WAITALL);
		recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
		printf("\nMessage from the server:%s\n", recvBuff);
	}
	else if(strcmp(argv[2],uploadCmd)==0)
	{
		file_path = argv[3];
		filefd = open(file_path, O_RDONLY);
		if (filefd == -1)
		{
        		perror("open");
			
			strcpy(sendBuff, "File not found\n");
			send(sockfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
			
			
        		exit(EXIT_FAILURE);
		}
		else
			{
				strcpy(sendBuff, "File uploading\n");
				send(sockfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
			}
		printf("uploading\n");
		while (1)
		{
        		read_return = read(filefd, buffer, sizeof(buffer));
        		if (read_return == 0)
            			break;
        		if (read_return == -1)
			{
            			perror("read");
            			exit(EXIT_FAILURE);
        		}
        		if (write(sockfd, buffer, read_return) == -1)
			{
            		perror("write");
            		exit(EXIT_FAILURE);
        		}
    		}
		close(filefd);
	}
	else if(strcmp(argv[2],downldCmd)==0)
	{
		file_path = argv[3];		
		filefd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        	if (filefd == -1) {
            				perror("open");
            				exit(EXIT_FAILURE);
        			}
		recv(sockfd, fileMessage, sizeof(recvBuff), MSG_WAITALL);
		printf("Message from the server: %s\n", fileMessage);
		if(strcmp(fileMessage,fileNotFound)==0)
		{
			close(filefd);
			rt = remove(file_path);
			exit(0);
		}
        	do 
		{
            		read_return = read(sockfd, buffer, sizeof(buffer));
            		if (read_return == -1) 
			{
                		perror("read");
                		exit(EXIT_FAILURE);
            		}
            		if (write(filefd, buffer, read_return) == -1)
			{
                		perror("write");
                		exit(EXIT_FAILURE);
            		}
        	} while (read_return > 0);
        	
		close(filefd);
	}
	return 0;
}
