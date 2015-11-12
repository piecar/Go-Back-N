#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFFSIZE 256
#define MAXPAY 1024
#define PACKSIZE 32 // 1B ACK, 6B seq#, 6B numPackets, 2B checksum, 3B for spaces
#define WINSIZE 100

void syserr(char *msg) { perror(msg); exit(-1); }
void ftpcomm(int newsockfd, char* buffer);
void readandsend(int tempfd, int newsockfd, char* buffer);
void recvandwrite(int tempfd, int newsockfd, int size, char* buffer);

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, pid, size;
  uint32_t seqNum;
  short checksum;
  struct sockaddr_in serv_addr, clt_addr;  
  struct stat filestats;
  socklen_t addrlen;
  char * recvIP;
  char buffer[BUFFSIZE];
  char payload[MAXPAY];

  if(argc != 3) { 
    fprintf(stderr,"Usage: %s <IP> <port> <name of file>\n", argv[0]);
    return 1;
  } 
  else
  { 
	server = gethostbyname(argv[1]);
	if(!server) {
	fprintf(stderr, "ERROR: no such host: %s\n", argv[1]);
	return 2;
	}
  	portno = atoi(argv[2]);
  }
  
  //Socket Logic
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) syserr("can't open socket"); 
  	printf("create socket...\n");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    syserr("can't bind");
  printf("bind socket to port %d...\n", portno);
  
  //Make array of packet elements to send
  stat(arvg[3], &filestats);
  size = filestats.st_size;
  numPackets = size/PACKSIZE + 1
  char * fileArray[numPackets];
  seqNum = 0;
  //Open File
  tempfd = open(arvg[3], O_RDWR);
  if(tempfd < 0) syserr("failed to open file");
  
  //Populate the array with packets to send
  while(1){
  	char * packet;
  	char str[13] //CHECK THE SIZE
  	checksum = 0;
  	packet = malloc(sizeof(char)*(PACKSIZE + MAXPAY));
  	char payload[MAXPAY];
  	
  	memset(packet, 0, (PACKSIZE + MAXPAY));
  	strcpy(packet, "0"); //Set ACK to 0
  	strcat(packet, " "); //Set space
  	sprintf(str, "%d", seqNum);
  	strcat(packet, str); //Set Sequence #
  	strcat(packet, " "); //Set Space
  	sprintf(str, "%d", numPackets);
  	strcat(packet, str); //Set number of packets
  	strcat(packet, " "); //Set Space
  	sprintf(str, "%d", checksum);
  	strcat(packet, str); //Set zero checksum
  	// Read payload from file, add to packet
  	int bytes_read = read(tempfd, payload, MAXPAY);
	buffer[bytes_read] = '\0';
	if (bytes_read == 0) // We're done reading from the file
		break;
	if (bytes_read < 0) syserr("error reading file");
	strcat(packet, payload); //Set Payload
	
	char *mkChkSum
  	
  }

for(;;) {
  printf("wait on port %d...\n", portno);
  addrlen = sizeof(clt_addr); 
  newsockfd = accept(sockfd, (struct sockaddr*)&clt_addr, &addrlen);
  if(newsockfd < 0) syserr("can't accept"); 
  
  pid = fork();
   if (pid < 0)
     syserr("Error on fork");
   if (pid == 0)
   {
     close(sockfd);
     ftpcomm(newsockfd, buffer);
     exit(0);
   }
   else
	 close(newsockfd);
}
  close(sockfd); 
  return 0;
}

unsigned int_to_int(unsigned k) {
    if (k == 0 || k ==1 ) return k;
    return (k % 2) + 10 * int_to_int(k / 2);
}

void ftpcomm(int newsockfd, char* buffer)
{
	int n, size, tempfd;
    struct stat filestats;
	char * filename;
	char command[20];
	filename = malloc(sizeof(char)*BUFFSIZE);
	for(;;)
	{
	    memset(buffer, 0, BUFFSIZE);
		n = recv(newsockfd, buffer, BUFFSIZE, 0);
		//printf("amount of data recieved: %d\n", n);
		if(n < 0) syserr("can't receive from client");
		sscanf(buffer, "%s", command);
		//printf("message from client is: %s\n", buffer);
		
		if(strcmp(command, "ls") == 0)
		{
			system("ls >remotelist.txt");
			stat("remotelist.txt", &filestats);
			size = filestats.st_size;
			//printf("Size of file to send: %d\n", size);
            size = htonl(size);      
			n = send(newsockfd, &size, sizeof(int), 0);
		    if(n < 0) syserr("couldn't send size to client");
			//printf("The amount of bytes sent for filesize is: %d\n", n);
			tempfd = open("remotelist.txt", O_RDONLY);
			if(tempfd < 0) syserr("failed to get file, server side");
			readandsend(tempfd, newsockfd, buffer);
			close(tempfd);			
		}
		
		if(strcmp(command, "exit") == 0)
		{
			printf("Connection to client shutting down\n");
			int i = 1;
			i = htonl(i);
			n = send(newsockfd, &i, sizeof(int), 0);
		    if(n < 0) syserr("didn't send exit signal to client");
			break;  // check to make sure it doesn't need to be exit
		}
		
		if(strcmp(command, "get") == 0)
		{
			sscanf(buffer, "%*s%s", filename);
			//printf("filename from client is: %s\n", filename);
			//printf("size of filename is: %lu\n", sizeof(filename));
			stat(filename, &filestats);
			size = filestats.st_size;
			//printf("Size of file to send: %d\n", size);
            size = htonl(size);      
			n = send(newsockfd, &size, sizeof(int), 0);
		    if(n < 0) syserr("couldn't send size to client");
			//printf("The amount of bytes sent for filesize is: %d\n", n);
			tempfd = open(filename, O_RDONLY);
			if(tempfd < 0) syserr("failed to open file");
			readandsend(tempfd, newsockfd, buffer);
			close(tempfd);			
		}
		if(strcmp(command, "put") == 0)
		{
			//Parse filename
			sscanf(buffer, "%*s%s", filename);
			//printf("filename from client is: %s\n", filename);
			//printf("size of filename is: %lu\n", sizeof(filename));
			//Receieve size of file
			n = recv(newsockfd, &size, sizeof(int), 0); 
        	if(n < 0) syserr("can't receive from server");
        	size = ntohl(size);        
			if(size ==0) // check if file exists
			{
				printf("File not found\n");
				break;
			}
			//printf("The size of the file to recieve is: %d\n", size);
			//Receieve the file
			tempfd = open(filename, O_CREAT | O_WRONLY, 0666);
			if(tempfd < 0) syserr("failed to open the file");
			recvandwrite(tempfd, newsockfd, size, buffer);
			close(tempfd);
		}
	}
}

void readandsend(int tempfd, int newsockfd, char* buffer)
{
	while (1)
	{
		memset(buffer, 0, BUFFSIZE);
		int bytes_read = read(tempfd, buffer, BUFFSIZE); //is buffer cleared here?
		buffer[bytes_read] = '\0';
		if (bytes_read == 0) // We're done reading from the file
			break;

		if (bytes_read < 0) syserr("error reading file");
		//printf("The amount of bytes read is: %d\n", bytes_read); 
		
		int total = 0;
		int n;
		int bytesleft = bytes_read;
		//printf("The buffer is: \n%s", buffer);
		while(total < bytes_read)
		{
			n = send(newsockfd, buffer+total, bytesleft, 0);
			if (n == -1) 
			{ 
			   syserr("error sending file"); 
			   break;
			}
			//printf("The amount of bytes sent is: %d\n", n);
			total += n;
			bytesleft -= n;
		}
	}
}

void recvandwrite(int tempfd, int newsockfd, int size, char* buffer)
{
	int totalWritten = 0;
	int useSize = 0;
	while(1)
	{
		if(size - totalWritten < BUFFSIZE) 
		{
			useSize = size - totalWritten;
		}
		else
		{
			useSize = BUFFSIZE;
		}
			memset(buffer, 0, BUFFSIZE);
			int total = 0;
			int bytesleft = useSize; //bytes left to recieve
			int n;
			while(total < useSize)
			{
				n = recv(newsockfd, buffer+total, bytesleft, 0);
				if (n == -1) 
				{ 
					syserr("error receiving file"); 
					break;
				}
				total += n;
				bytesleft -= n;
			}
			//printf("The buffer is: \n%s", buffer);
			//printf("Amount of bytes received is for one send: %d\n", total);
		
			int bytes_written = write(tempfd, buffer, useSize);
			//printf("Amount of bytes written to file is: %d\n", bytes_written);
			totalWritten += bytes_written;
			//printf("Total amount of bytes written is: %d\n", totalWritten);
			if (bytes_written == 0 || totalWritten == size) //Done writing into the file
				break;

			if (bytes_written < 0) syserr("error writing file");
		
    }	
}
