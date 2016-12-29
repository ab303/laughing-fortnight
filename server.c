#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#define deleteCmd "delete"
#define renameCmd "rename"
#define uploadCmd "upload"
#define downloadCmd "downld"
#define fileNotFound "File not found\n" 

void threadFunc(int sockd)
{
	
		

		int  connfd = 0, rt=0, filefd = 0;
     		char sendBuff[256], recvBuff[256] , fileName[256], newName[256], buffer[1024], fileMessage[256];
    		char *file_path = "o";
    		time_t ticks;
    		ssize_t read_return;
		

		connfd = sockd;
	
		recv(connfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
	        printf("Operation required %s\n", recvBuff);
	        strcpy(sendBuff, "Server ACK for operation\n");
	        send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
		
		recv(connfd, fileName, sizeof(recvBuff), MSG_WAITALL);
	        printf("Filename: %s\n", fileName);
		strcpy(sendBuff, "Server ACK for Filename\n");
	        send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
	
		if(strcmp(recvBuff,deleteCmd)==0)
		{
			printf("deleting filename:%s\n", fileName); 
	   		rt = remove(fileName);

	   		if(rt == 0) 
	   		{
	      			strcpy(sendBuff, "File deleted successfully\n");
				send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
				pthread_exit(0);
   			}
   			else 
   			{
     				strcpy(sendBuff, "unable to delete file\n");
				send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
				pthread_exit(0);
   			}
		}
		else if(strcmp(recvBuff,renameCmd)==0)
		{	
		
			strcpy(sendBuff, "Please enter the new filename\n");
			send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
			recv(connfd, newName, sizeof(recvBuff), MSG_WAITALL);

			rt = rename(fileName, newName);
	
	   		if(rt == 0) 
	   		{
	      			strcpy(sendBuff, "File renamed successfully\n");
				send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
				pthread_exit(0);
	   		}
	   		else 
	   		{
	      			strcpy(sendBuff, "unable to rename\n");
				send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
				pthread_exit(0);
	   		}
		}
		else if(strcmp(recvBuff, uploadCmd)==0)
		{
			
			file_path = fileName;		
			filefd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	        	if (filefd == -1) {
	            				perror("open");
	            				exit(EXIT_FAILURE);
	        			}
			recv(connfd, fileMessage, sizeof(recvBuff), MSG_WAITALL);
			if(strcmp(fileMessage,fileNotFound)==0)
			{
				close(filefd);
				rt = remove(file_path);
				printf("no such file exits at client\n");
				pthread_exit(0);
			}
			printf("%s" , fileMessage);
	        	do 
			{
	            		read_return = read(connfd, buffer, sizeof(buffer));
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
			pthread_exit(0);
		}
		else if(strcmp(recvBuff,downloadCmd)==0)
		{
			file_path = fileName;
			filefd = open(file_path, O_RDONLY);
			if (filefd == -1)
			{
	        		perror("open");

				strcpy(sendBuff, "File not found\n");
				send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
				
        			exit(EXIT_FAILURE);
			}
			else
			{
				strcpy(sendBuff, "File downloading\n");
				send(connfd, sendBuff, sizeof(recvBuff), MSG_WAITALL);
			}
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
	        		if (write(connfd, buffer, read_return) == -1)
				{
		            		perror("write");
		            		exit(EXIT_FAILURE);
	        		}
	    		}
			close(filefd);
			pthread_exit(0);	
		}
		else
		{	
			printf("no matching command found\n");
			pthread_exit(0);

        
		}
	}


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0, i=0;
    struct sockaddr_in serv_addr; 
    char sendBuff[256], recvBuff[256];
    time_t ticks;
    pthread_t tid[50];
    				

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 
    memset(recvBuff, '0', sizeof(recvBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8080); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 50); 

    while(i<50)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        
	pthread_create(&tid[i], NULL, threadFunc, connfd);
	i++;

    }
    for(i=0;i<50;i++)
	{
		pthread_join(tid[i], 0);
	}
	
	
	close(connfd);
        sleep(1);


}
