#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFFSIZE 256
#define PACSIZE 1040
#define ACKSIZE 16

void syserr(char *msg) { perror(msg); exit(-1); }
uint16_t ChkSum(char * packet, int psize);
void ftpcomm(int newsockfd, char* buffer);
void readandsend(int tempfd, int newsockfd, char* buffer);
void recvandwrite(int tempfd, int newsockfd, int size, char* buffer);

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, pid, numPacketsEmpty, cltPort;
  uint32_t seqNum, exSeqNum, numPackets;
  uint16_t checksum;
  uint8_t ack;
  struct sockaddr_in serv_addr, clt_addr;
  struct timeval tv;
  fd_set readfds;
  socklen_t addrlen;
  char * recvIP;
  char buffer[256];
  char packet[PACSIZE];
  char ackPac[ACKSIZE];

  if(argc != 3) { 
    fprintf(stderr,"Usage: %s <port> <name of file>\n", argv[0]);
    return 1;
  } 
  else
  { 
  	portno = atoi(argv[1]);
  }

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) syserr("can't open socket"); 
  	printf("create socket...\n");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  addrlen = sizeof(clt_addr); 

  if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    syserr("can't bind");
  printf("bind socket to port %d...\n", portno);
  
  //Set up to receive
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  FD_ZERO(&readfds);
  FD_SET(sockfd, &readfds);
  tempfd = open(argv[2], O_CREAT | O_WRONLY, 0666);
  if(tempfd < 0) syserr("failed to open the file");
  memset(packet, 0, PACSIZE);
  memset(ackPac, 0, ACKSIZE);
  exSeqNum = 0;
  numPacketsEmpty = 1;
  
  //Receive packet
  while(1){
  	select(sockfd+1, &readfds, NULL, NULL, &tv);
  	if(FD_ISSET(sockfd, &readfds)){
	  n = recvfrom(sockfd, packet, PACSIZE, 0, 
	  	(struct sockaddr*)&clt_addr, &addrlen);
	  if(n < 0) syserr("can't receive from sender"); 
	  
	  seqNum = (uint32_t) packet[5] | (uint32_t) packet[4] << 8 | 
	  		   (uint32_t) packet[3] << 16 | (uint32_t) packet[2] << 24;
	  checksum = ChkSum(packet, PACSIZE);
	  if(numPacketsEmpty){
	  	haveNumPackets = 0;
	  	numPackets = (uint32_t) packet[10] | (uint32_t) packet[9] << 8 | 
	  			(uint32_t) packet[8] << 16 | (uint32_t) packet[7] << 24;
	  }
	  
	  //Check if packet is correct
	  if(checksum == 0 && seqNum == exSeqNum){
	  	ack = 1;
	  	//set ack
	  	ackPac[0] = ack & 255;
	  	ackPac[1] = ' ';
	  	//set seq#
	  	ackPac[2] = (seqNum >>  24) & 255;
	  	ackPac[3] = (seqNum >>  16) & 255;
	  	ackPac[4] = (seqNum >>  8) & 255;
	  	ackPac[5] = seqNum & 255;
	  	ackPac[6] = ' ';
	  	//set numPackets
	  	ackPac[7] = (numPackets >>  24) & 255;
	  	ackPac[8] = (numPackets >>  16) & 255;
	  	ackPac[9] = (numPackets >>  8) & 255;
	  	ackPac[10] = numPackets & 255;
	  	ackPac[11] = ' ';
	  	//set checksum
	  	ackPac[12] = (checksum >>  8) & 255;
	  	ackPac[13] = checksum & 255;
	  	ackPac[14] = ' ';
	  	ackPac[15] = ' ';
	  	
	  	//calculate and set checksum
	  	checksum = ChkSum(packet, PACSIZE);
	  	ackPac[12] = (checksum >>  8) & 255;
	  	ackPac[13] = checksum & 255;
	  	
	  	n = sendto(sockfd, ackPac, ACKSIZE, 0, 
	  		(struct sockaddr*)&clt_addr, addrlen);
  		if(n < 0) syserr("can't send to receiver");
	  	
	  	exSeqNum++;
	  	
	  	//Write packet to file
	  	write(tempfd, buffer, useSize);
	  }
	  else if(exSeqNum > 0){ //packet fault, but past first packet
	  	 n = sendto(sockfd, ackPac, ACKSIZE, 0, 
	  		 (struct sockaddr*)&clt_addr, addrlen);
  		 if(n < 0) syserr("can't send to receiver");
	  }
	  
	}
	else{
		if(seqNum != numPackets)
			printf("File not received, timeout after 60 secs\n");
		else
			printf("File receieved, timeout after 60 secs\n");
	}
  }
  
  
  close(sockfd); 
  return 0;
}

uint16_t ChkSum(char * packet, int psize){
	uint16_t checksum = 0, curr = 0, i = 0;
	//sscanf(packet, "%*s %*s %*s %u", &checksum);
	//printf("checksum is: %d. Packet Size is: %d\n", checksum, psize);
	while(psize > 0){
		curr = ((packet[i] << 8) + packet[i+1]) + checksum;
		checksum = curr + 0x0FFFF;
		curr = (curr >> 16); //Grab the carryout if it exists
		checksum = curr + checksum;
		psize -= 2;
		i += 2;
	}
	return ~checksum;
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
