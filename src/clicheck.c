#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

int sockfd, portnum, n;
int exitconn=1;
char iobuff[500];
struct sockaddr_in server_addr;	  //sockaddr_in - structure that helps you to reference to the socket's elements
struct hostent *host;		      //hostent - structure is used to keep information related to host

int write (int, const void *, int);
int read (int, const void *, int);
int estab_connection(int, char *[]);
int transferdata(int);    

void main(int argc, char *argv[])
{
	if (argc < 3) 
	{
      printf("usage %s hostname port\n", argv[0]); //Checking if the user input is in the valid format
      exit(0); 
   	}
   	
    sockfd = estab_connection(argc, argv); //call to establish a connection
    
  	// while an exit indication is not received, continue to process client commands	
    while(exitconn)
    	exitconn = transferdata(sockfd);
}

int estab_connection(int argc, char *argv[])
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);	//This call returns a socket descriptor that you can use in later system calls or -1 on error.

    if (sockfd < 0) {		//Check for error
        printf("ERROR: Could not open socket \n");
        exit(1);
    }
    portnum = atoi(argv[2]); //changing the portnumber to integer
    
    host = gethostbyname(argv[1]);	/* The gethostbyname() function returns a structure of type hostent for
					the given host name.  Here name is either a hostname or an IPv4 address in standard dot notation*/
					
	//check for errors
    if (host == NULL) {
        printf("ERROR: Input host not found.\n");
        exit(0);
    }

	//initialize client
    bzero((char *) &server_addr, sizeof(server_addr));	//initialize to zero to avoid garbage values

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    bcopy((char *)host->h_addr, (char *)&server_addr.sin_addr.s_addr, host->h_length);

    // Connecting to server-Request 
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR: Could not connect to server\n");
        exit(1);
    }
    printf("Connected to Server \n ");
    return sockfd; // returning the file descriptor obtained for data transfer
}

int transferdata(int sockfd)
{
    printf("> "); //a probe for the user to enter commands
    
    bzero(iobuff,500); //empty the buffer for the user command
    fgets(iobuff,500,stdin);
    
    // Write/send command to server
    n = write(sockfd, iobuff, strlen(iobuff));
   
    if (n < 0) //check for errors
    {
        printf("ERROR: Could not write to socket");
        exit(1);
    }

    //empty the buffer before reading the server output
    bzero(iobuff,500);
    
    // Reading server output
    n = read(sockfd, iobuff, 500);

	// check if exit command is entered, if yes close the conection
	if (strncmp(iobuff,"exit",4)==0)
    {	
    	close(sockfd);
    	printf("Connection Terminated from server\n");
    	return 0;
    }
	
    if (n < 0) //check for errors
    {
        printf("ERROR: Could not read from socket");
        exit(1);
    }
    
    printf("%s\n",iobuff);  //printing the final output received from server
    return 1;
}
