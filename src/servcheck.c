#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

int sockfd, newsockfd, client_len, portnum, n, i, c;
int exitconn=1;
char iobuff[500];
FILE *file_pointer;
struct sockaddr_in server_addr, client_addr;

int write (int, const void *, int );
int read (int, const void *, int);
int estab_connection(int, char *[]);
int transferdata(int);


void main(int argc, char *argv[])
{
    	
	if (argc < 2)  //Checking if the user input is in the valid format
	{
      printf("usage %s port\n", argv[0]);
      exit(0);
   	}
   	
    l1:		newsockfd = estab_connection(argc, argv); //call to establish a connection
    		printf("Connected to Client \n");
    		
    	// while an exit indication is not received, continue to process client commands	
		while(exitconn)
		{
			exitconn=transferdata(newsockfd);
			if (exitconn==2)
				goto l1; //if an exit indication is received, go to establishing a new connection
		}
}

int estab_connection(int argc, char *argv[])
{
	//if there is an existing socket which is binded, directly start to wait for a new client
	if(exitconn==2) 
	{
		printf("Waiting for client \n");
		goto l2;
	}
	
    portnum = atoi(argv[1]); //convert the portnumber recevied to an integer
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //returns a socket descriptor; returns -1 on error

    if (sockfd < 0) {
        printf("ERROR: Could not open socket");
        exit(1);
    }
    
    printf("Waiting for client \n");

    // Initialize socket
    
    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portnum);

	//bind assign local protocol address to socket; returns 0 if successful
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("ERROR: Could not bind server");
        exit(1);
    }
    //Awaiting client for further instructions via listen:
    l2: listen(sockfd, 5);
    client_len = sizeof(client_addr);

    //Accept connection request from client 
    newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
    if (newsockfd < 0) {
        printf("ERROR: Could not accept client connection");
        exit(1);
    }
	
    return newsockfd; //return the connected socket file descriptor
}

int transferdata(int newsockfd)
{
    //Read the client's request using the obtained file descriptor
    bzero(iobuff,500);
    n = read(newsockfd,iobuff,500);
   
   //Check if the exit command is entered
    if (strncmp(iobuff,"exit",4)==0)
       n = write(newsockfd,iobuff,500); //pass the command back to client
       
	//close the connection with present client and return 2 indicating to wait for a new connection       
	if (n==0) 
   	{
   		close(newsockfd);
   		return 2;
   	} 
   	
    if (n < 0) //check for error
    {
        printf("ERROR: Can not read from socket");
        exit(1);
    }

/*	If exit is not entered we proceed.
	popen() process used to execute system command and 
	store the corresponding output to a file indicated
	by the file pointer.
*/

    file_pointer = popen(iobuff, "r"); 
    if (file_pointer == NULL)
        printf("ERROR: could not process command");
	
	bzero(iobuff,500); // clear the buffer for writing new results
	
	//Copying character by character the whole file to the buffer
	//Had to do this since fgets stopped reading after it encountered a new line
	for (i = 0; i < 500; ++i)
	{
    	c = getc(file_pointer);
    	if (c == EOF)
		{	
			pclose(file_pointer);
			break;
		}
		else
    	iobuff[i] = c;
	}
	
    // Send response back to client
	n = write(newsockfd,iobuff,500);

    if (n < 0) {
        printf("ERROR: could not write to socket");
        exit(1); //checking for errors
    }

}
