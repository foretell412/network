#include <sys/socket.h>      
#include <sys/types.h>       
#include <arpa/inet.h>       
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#define MAX_LINE  (1000)
#define BUFSIZE (1024)

int gameover=1;


ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char    c, *buffer;
    
    buffer = vptr;
    
    for ( n = 1; n < maxlen; n++ ) {
        
        if ( (rc = read(sockd, &c, 1)) == 1 ) {
            *buffer++ = c;
            if ( c == '\n' )
                break;
        }
        else if ( rc == 0 ) {
            if ( n == 1 )
                return 0;
            else
                break;
        }
        else {
            if ( errno == EINTR )
                continue;
            return -1;
        }
    }
    
    *buffer = 0;
    return n;
}

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
    size_t      nleft;
    ssize_t     nwritten;
    const char *buffer;
    
    buffer = vptr;
    nleft  = n;
    
    while ( nleft > 0 ) {
        if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
            if ( errno == EINTR )
                nwritten = 0;
            else
                return -1;
        }
        nleft  -= nwritten;
        buffer += nwritten;
    }
    
    return n;
}
void send_recv(int i, int sockfd,int *wait)
{
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
	
	if (i == 0){
        
		fgets(send_buf, BUFSIZE, stdin);
        
        if (send_buf[strlen(send_buf)-1]=='\n'){
            send_buf[strlen(send_buf)-1]='\0';
        }
		if (strcmp(send_buf , "quit\n") == 0) {
			exit(0);
		}else
			send(sockfd, send_buf, strlen(send_buf), 0);
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n" , recv_buf);
        if ((recv_buf[0]=='g')&&(recv_buf[1]=='o')){
            printf("game over\n");
            gameover=1;
        }
        
		fflush(stdout);
	}
}

int PrintMenu(){
    printf("===============================\n");
    printf("  Menu\n");
    printf("1 Sign up\n");
    printf("2 Login\n");
    printf("3 Exit\n");
    printf("===============================\n");

}
int playMenu(){
    printf("===============================\n");
    printf("  Game\n");
    printf("1 Online player\n");
    printf("2 Connect and play\n");
    printf("3 Sign out\n");
    printf("===============================\n");

}
int main(int argc, char *argv[]) {
    
    int       conn_s;                
    short int port;                 
    struct    sockaddr_in servaddr;  
    char      buffer[MAX_LINE]; 
    char recvbuf[MAX_LINE];        
    char     *endptr;                   
    struct hostent *host;
    char user[20];
    char passwd[20];
    int op;int gameop=0;
    char success[20];
    char fail[20];
    int recvbyte;
    char rival[20];
    int onlstate=0;
    fd_set master;
	fd_set read_fds;
    int fdmax;
    int i,j;
    int wait=0;
    
    
    port = strtol(argv[2], &endptr, 0);
    if ( *endptr ) {
        printf("ECHOCLNT: Invalid port supplied.\n");
        exit(EXIT_FAILURE);
    }
    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);
    
    if((host=gethostbyname(argv[1]))==NULL){
        perror("gethostbyname");
        exit(-1);
    }
    servaddr.sin_addr=*((struct in_addr*)host->h_addr);
    memset(&(servaddr.sin_zero),'\0',8);
    
    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
        printf("ECHOCLNT: Error calling connect()\n");
        exit(EXIT_FAILURE);
    }
    
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0, &master);
    FD_SET(conn_s, &master);
	fdmax = conn_s;

    int quit=0;
    while(quit==0)
    {
        PrintMenu();
        scanf("%d",&op);
        switch (op){
            case 1:{
                printf("Enter user name: ");
                scanf("%s",user);
                printf("Enter password: ");
                scanf("%s",passwd);
                strcpy(buffer,"su");
                strcat(buffer,user);
                strcat(buffer," ");
                strcat(buffer,passwd);
                printf("Message: %s\n",buffer);
                Writeline(conn_s,buffer,strlen(buffer));
                int recvbyte=recv(conn_s,recvbuf,sizeof(recvbuf),0);
                printf("recvbyte=%d\n",recvbyte);
                recvbuf[recvbyte]='\0';
                printf("%s",recvbuf);
                break;
            }
            case 2:{
                
                printf("Enter user name: ");
                scanf("%s",user);
                printf("Enter password: ");
                scanf("%s",passwd);
                strcpy(buffer,"lg");
                strcat(buffer,user);
                strcat(buffer," ");
                strcat(buffer,passwd);
                printf("Message: %s\n",buffer);
                Writeline(conn_s,buffer,strlen(buffer));
                recvbyte=recv(conn_s,recvbuf,sizeof(recvbuf),0);
                printf("recvbyte=%d\n",recvbyte);
                recvbuf[recvbyte]='\0';
                printf("%s",recvbuf);
                strcpy(success,"success");
                strcpy(fail,"fail");
                if (strstr(recvbuf,success)!=NULL){
                    while(gameop!=3){
                        playMenu(); 
                        scanf("%d",&gameop);
                        
                        switch (gameop){
                            case 1:{
                                //display list of user
                                strcpy(buffer,"ol");
                                Writeline(conn_s,buffer,strlen(buffer));
                                recvbyte=recv(conn_s,recvbuf,sizeof(recvbuf),0);
                                printf("recvbyte=%d\n",recvbyte);
                                recvbuf[recvbyte]='\0';
                                printf("%s",recvbuf);
                                break;
                            }
                            case 2:{
                                //check whether there are some challenge message
                                gameover=0;
                                while((wait==0)&&(gameover==0)){
                                    read_fds = master;
                                    if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
                                        perror("select");
                                        exit(4);
                                    }
                                    
                                    for(i=0; i <= fdmax; i++ )
                                        if(FD_ISSET(i, &read_fds))
                                            send_recv(i, conn_s,&wait);
                                }
                                gameover=0;
                                break;
                            }
                            case 3:{
                                //sign out
                                strcpy(buffer,"so");
                                Writeline(conn_s,buffer,strlen(buffer));
                                
                                printf("sign out\n");
                                break;
                            }
                            default:{
                                break;
                            }
                        } 
                    }
                    gameop=0;
                    
                }
                break;
            }
                
            
            case 3:{
                quit=1;
                break;
            }
            defaut:{
                quit=1;
                break;
            }
        }
    }
    
        
    return EXIT_SUCCESS;
}

