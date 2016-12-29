# multithreaded-file-server
This is an implementaion of single server and multiple clients model.
Client can request server to upload, download, rename and delete files.
Server handles the requests from different clients simultaneously by multi threading.
resouce locking mechanisms have not been used.

To compile the programs:

type:

gcc client.c -o client

gcc server.c -lpthread -o server



To run the program:



1. Start the server

 - ./server


2. Run the client

 - ./client SERVER_IP <command> <target-filename>



Note : 
 

* The 4 commands are :

 - Delete - "delete"

 - Rename - "rename"

 - Upload - "upload"

 - Download - "downld"
	

* Sample Command : 
./client 192.168.10.11 downld abc.txt
 

* The filename must not contain any spaces.
