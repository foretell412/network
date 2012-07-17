#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MYPORT 1234
#define MAX_BUF 256
#define USERFILE "userfile.txt"
#define ONLINEFILE "online.txt"
#define LIBRARY "library.txt"
FILE * loadUserList(char filename[20],char user[20][100],char pass[20][100],int *count){
    FILE * file;
    file=fopen(filename,"r");
    if (file==NULL){
        perror("Can not open file\n");
        exit(0);
    }
    int c=0;
    while(!feof(file)){
        fscanf(file,"%s",user[c]);
        fscanf(file,"%s",pass[c]);
        c++;        
    }
    *count=c;
    return file;
}
FILE * loadOnlineList(char filename[20],char onluser[20][100],char onlpass[20][100],int socket[100],int * count){
    FILE * file;
    file=fopen(filename,"r");
    if (file==NULL){
        perror("Can not open file\n");
        exit(0);
    }
    int c=0;
    while(!feof(file)){
        fscanf(file,"%s",onluser[c]);
        fscanf(file,"%s",onlpass[c]);
        fscanf(file,"%d",&socket[c]);
        c++;
        
    }
    *count=c;
    return file;
}
FILE * loadQuestion(char filename[20],char question[20][1000],char answer[20][1000],int * qcount){
    FILE *file=fopen(filename,"r");
    if (file==NULL){
        perror("Can not open file\n");
        exit(0);
    }
    int c=0;int i;
    while(!feof(file)){
        fscanf(file,"%s",question[c]);
        fscanf(file,"%s",answer[c]);
        //printf("%s %s \n",question[0],answer[0]);
        c++;
        
        
    }
    
    *qcount=c-1;
    /*
    for (i=0;i<c-1;i++){
        printf("%s %s \n",question[i],answer[i]);
    }
     */
    printf("ok\n");
    fclose(file);
    return file;
}
int main(int argc,char* argv[]){
    fd_set main_fds,read_fds;
    struct sockaddr_in myaddr;
    struct sockaddr_in theiraddr;
    int fdmax;
    int listener;
    int newfd;
    int addrlen;
    char buf[MAX_BUF];
    int nbytes;
    int yes=1;int q1,q2;
    int i,j,w;
    int port;
    char *endptr;
    FILE * userfile;
    FILE * onlfile;
    char filename[20];
    char command[20];
    char sub[20];
    char user[20];
    char passwd[20];
    int state;
    int sock;
    int nowsock;
    int count;int onlcount;
    char userlist[20][100];
    char passlist[20][100];
    char onluser[20][100];
    char onlpass[20][100];
    int onlsock[100];
    int onlstate[100];
    char challenger[20];
    char question[20][1000];
    char answer[20][1000];
    int x;int y;int exist;int z;int flag;int timeup=0;
    int gamestart=0;int u1,u2;
    int truesock;
    int rivalsock;
    int qcount=0;
    int questionid[100];
    char message[20];
    char rival[20];
    char ans[200];
    int seq1=0;
    int seq2=0;
    for (i=0;i<100;i++){
        onlstate[i]=0;
    }
    //delete information in onlinefile
    onlfile=fopen(ONLINEFILE,"w");
    fclose(onlfile);
    
    loadUserList(USERFILE,userlist,passlist,&count);
    count--;
    printf("user count=%d\n",count);
    
    
    loadOnlineList(ONLINEFILE,onluser,onlpass,onlsock,&onlcount);
    onlcount--;
    printf("onlcount=%d\n",onlcount);
    for (i=0;i<onlcount;i++){
        printf("%s %s %d\n",onluser[i],onlpass[i],onlsock[i]);
    }
//----------------------------------------------------------------
    loadQuestion(LIBRARY,question,answer,&qcount);
    for (i=0;i<qcount-1;i++){
        printf("%s %s \n",question[i],answer[i]);
    }
     
    
    FD_ZERO(&main_fds);
    FD_ZERO(&read_fds);
    if((listener=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        exit(1);
    }
    
    if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
        perror("setsockopt");
        exit(1);
    }
    //bind
    myaddr.sin_family=AF_INET;
    myaddr.sin_addr.s_addr=INADDR_ANY;
    if (argc<1) myaddr.sin_port=htons(MYPORT);
    else {
        port = strtol(argv[1], &endptr, 0);
        if ( *endptr ) {
            printf("ECHOCLNT: Invalid port supplied.\n");
            exit(EXIT_FAILURE);
        }
        myaddr.sin_port=htons(port);
    }
    memset(&(myaddr.sin_zero),'\0',8);
    if(bind(listener,(struct sockaddr *)&myaddr,sizeof(myaddr))==-1){
        perror("bind");
        exit(1);
    }
    //listen
    if(listen(listener,10)==-1){
        perror("listen");
        exit(1);
    }
    //??????????
    FD_SET(listener,&main_fds);
    fdmax=listener;
    printf("fdmax=%d\n",fdmax);
    while(1){
        read_fds=main_fds;
        
        if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1){
            perror("select");
            exit(1);
        }
        for(i=0;i<=fdmax;i++){
            if(FD_ISSET(i,&read_fds)){
                // create new connection
                if(i==listener){
                    addrlen=sizeof(theiraddr);
                    if((newfd=accept(listener,(struct sockaddr *)&theiraddr,&addrlen))==-1){
                        perror("accept");
                    }else{
                        FD_SET(newfd,&main_fds);
                        if(newfd>fdmax){
                            fdmax=newfd;
                            printf("fdmax=%d\n",fdmax);
                        }
                        printf("New connection from %s - Socket %d\n",inet_ntoa(theiraddr.sin_addr),newfd);
                    }
                //close socket     
                }else{ 
                    nbytes=recv(i,buf,sizeof(buf),0);
                    nowsock=i;
                    printf("nowsock=%d\n",nowsock);
                    printf("onlsock[0]=%d\n",onlsock[0]);
                    buf[nbytes]='\0';
                    printf("buf=%s\n",buf);
                                        
                    if(nbytes<=0){
                        if(nbytes==0){
                            printf("Socket %d Closed\n",i);
                        }else{
                            perror("recv");
                        }
                        close(i);
                        //
                        
                        for (w=0;w<onlcount;w++)
                        {
                            if (onlsock[w]==i){
                                for (z=w;z<onlcount-1;z++){
                                    strcpy(onluser[z],onluser[z+1]);
                                    strcpy(onlpass[z],onlpass[z+1]);
                                    onlsock[z]=onlsock[z+1];
                                }
                                onlcount--;
                                w--;
                            }
                        }
                        onlfile=fopen(ONLINEFILE,"w");
                        for (w=0;w<onlcount;w++)
                        {
                            fprintf(onlfile,"%s %s %d\n",onluser[w],onlpass[w],onlsock[w]);
                        }
                        fclose(onlfile);
                         
                        FD_CLR(i,&main_fds);
                    }else{
                        sub[0]=buf[0];
                        sub[1]=buf[1];
                        sub[2]='\0';
                        printf("sub=%s\n",sub);
                        
                        printf("truoc su onlsock[0]=%d\n",onlsock[0]);
                        //if sign up
                        if (strcmp(sub,"su")==0)
                        {
                            
                            x=2;
                            while(buf[x]!=' '){
                                user[x-2]=buf[x];
                                x++;
                            }
                            user[x-2]='\0';
                            
                            printf("user=%s\n",user);
                            x++;y=0;
                            while(x<=strlen(buf)){
                                passwd[y++]=buf[x++];
                            }
                            printf("passwd=%s\n",passwd);
                            exist=0;
                            for (z=0;z<count;z++){
                                if (strcmp(user,userlist[z])==0){
                                    exist=1;
                                }
                            }
                            //printf("exist=%d\n",exist);
                            if (exist==0){
                                printf("Sign up success\n");
                                strcpy(buf,"Sign up success\n");
                                userfile=fopen(USERFILE,"a+");
                                strcpy(userlist[count],user);
                                strcpy(passlist[count],passwd);
                                count++;
                                fprintf(userfile,"%s %s\n",user,passwd);
                                fclose(userfile); 
                            }else{
                                printf("User existed\n");
                                strcpy(buf,"User existed\n");

                            }
                            if (send(i,buf,strlen(buf),0)==-1){
                                perror("Can not send message");
                            }
                        }
                        //if login
                        printf("truoc login onlsock[0]=%d\n",onlsock[0]);
                        if (strcmp(sub,"lg")==0){
                            x=2;
                            while(buf[x]!=' '){
                                user[x-2]=buf[x];
                                x++;
                            }
                            user[x-2]='\0';
                            
                            printf("login user=%s\n",user);
                            
                            x++;y=0;
                            while(x<=strlen(buf)){
                                passwd[y++]=buf[x++];
                            }
                            printf("login passwd=%s\n",passwd);
                            int truepass=0;
                            
                            for (z=0;z<count;z++){
                                if ((strcmp(user,userlist[z])==0)&&(strcmp(passwd,passlist[z])==0)){
                                    //printf("count=%d\n",count);
                                    truepass=1;
                                }
                                //if (nowsock==)
                            }
                            
                            if (truepass==0){
                                printf("login false\n");
                                strcpy(buf,"login false\n");
                            }else{
                                truesock=1;
                                for (z=0;z<onlcount;z++)
                                {
                                    if (onlsock[z]==nowsock){
                                        truesock=0;
                                    }
                                }
                                if (truesock==1){
                                    printf("login success\n");
                                    strcpy(buf,"login success\n");
                                    onlfile=fopen(ONLINEFILE,"a+");
                                    fprintf(onlfile,"%s %s %d\n",user,passwd,nowsock);
                                    fclose(onlfile);
                                    strcpy(onluser[onlcount],user);
                                    strcpy(onlpass[onlcount],passwd);
                                    onlsock[onlcount]=nowsock;
                                    printf("nowsock=%d\n",nowsock);
                                    onlcount++;
                                    

                                }
                                                            }
                            printf("login count=%d\n",onlcount);
                            if (send(i,buf,strlen(buf),0)==-1){
                                perror("Can not send message");
                            } 
                            
                        }
                        printf("truoc onl onlsock[0]=%d\n",onlsock[0]);
                        //display online player
                        if (strcmp(sub,"ol")==0){
                            printf("online people=%d\n",onlcount);
                            strcpy(buf,"");
                            for (w=0;w<onlcount;w++){
                                strcat(buf,onluser[w]);
                                printf("%s %d\n",onluser[w],onlsock[w]);
                                strcat(buf,"\n");
                                
                            }
                            if (send(i,buf,strlen(buf),0)==-1){
                                perror("Can not send message");
                            }    
                        }
                        printf("truoc request onlsock[0]=%d\n",onlsock[0]);
                        printf("buf=%s\n",buf);
                        
                        //check challenge request from player
                        if (strcmp(sub,"rq")==0){
                            
                            x=2;
                            printf("sau request1 onlsock[0]=%d\n",onlsock[0]);
                            while((buf[x]!=' ')&&(buf[x]!='\0')){
                                rival[x-2]=buf[x];
                                x++;
                            }
                            
                            rival[x-2]='\0';
                            printf("rival=%s\n",rival);
                            printf("user count: %d\n",onlcount); 
                            printf("sau request2 onlsock[0]=%d\n",onlsock[0]);

                            flag=0;
                            for (z=0;z<onlcount;z++){
                                if (strcmp(onluser[z],rival)==0) {
                                    rivalsock=onlsock[z];
                                    if (onlstate[z]!=0) flag=1; 
                                    else if (onlstate[z]==0) {
                                        onlstate[z]=1;
                                    }                                
                                }
                                if (onlsock[z]==nowsock) {
                                    strcpy(challenger,onluser[z]);
                                }
                            }
                            
                            
                            printf("flag=%d\n",flag);
                            if (flag==0){
                                strcpy(message,"cl");
                                strcat(message,challenger);
                                printf("send request=%s\n",message);
                                printf("rivalsock=%d\n",rivalsock);
                                if (send(rivalsock,message,strlen(message),0)==-1){
                                    perror("can not send challenge\n");
                                }
                            }
                            
                            //decline if rival received challenge from another
                            else if (flag==1){
                                strcpy(message,"dc");
                                strcat(message,rival);
                                if (send(nowsock,message,strlen(message),0)==-1){
                                    perror("can not send challenge\n");
                                }
                            }
                                                        
                         
                        }
                        // accept message
                        
                        if (strcmp(sub,"ac")==0){
                            x=2;
                            while((buf[x]!=' ')&&(buf[x]!='\0')){
                                rival[x-2]=buf[x];
                                x++;
                            }
                            rival[x-2]='\0';
                            printf("accept rival=%s\n",rival);
                            printf("onlcount=%d\n",onlcount);
                            //game start message
                            strcpy(message,"gt");
                            gamestart=1;
                            for (z=0;z<onlcount;z++){
                                printf("user %s sock %d\n",onluser[z],onlsock[z]);
                                if (strcmp(rival,onluser[z])==0){
                                    
                                    rivalsock=onlsock[z];
                                }
                            }
                            
                            if (send(nowsock,message,strlen(message),0)==-1){
                                perror("can not start \n");
                            }
                            
                            if (send(rivalsock,message,strlen(message),0)==-1){
                                perror("can not send challenge\n");
                            }
                            strcpy(message,"qu");
                            strcat(message,question[0]);
                            q1=0;q2=0;
                            u1=rivalsock;
                            u2=nowsock;
                            send(u1,message,strlen(message),0);
                            send(u2,message,strlen(message),0);
    
                        }
                        if ((strcmp(sub,"as")==0)&&(gamestart==1)){
                            x=2;
                            while((buf[x]!=' ')&&(buf[x]!='\0')){
                                ans[x-2]=buf[x];
                                x++;
                            }
                            ans[x-2]='\0';

                            printf("answer=%s\n",ans);
                            if (nowsock==u1){
                                if(strcmp(ans,answer[q1])==0){
                                    strcpy(message,"qu");
                                    q1++;
                                    strcat(message,question[q1]);
                                    send(u1,message,strlen(message),0);
                                    seq1++;
                                    printf("seq1 %d\n",seq1);
                                }
                                else{
                                    seq1=0;
                                }
                            }
                            if (nowsock==u2){
                                if (strcmp(ans,answer[q2])==0){
                                    strcpy(message,"qu");
                                    q2++;
                                    strcat(message,question[q2]);
                                    send(u2,message,strlen(message),0);
                                    seq2++;
                                    printf("seq2 %d\n",seq2);

                                }
                                else {
                                    seq2=0;
                                }
                            }                          
                        }
                        if ((strcmp(sub,"so")==0)&&(gamestart!=1)){
                            for (w=0;w<onlcount;w++)
                            {
                                if (onlsock[w]==i){
                                    for (z=w;z<onlcount-1;z++){
                                        strcpy(onluser[z],onluser[z+1]);
                                        strcpy(onlpass[z],onlpass[z+1]);
                                        onlsock[z]=onlsock[z+1];
                                    }
                                    onlcount--;
                                    w--;
                                }
                            }
                            onlfile=fopen(ONLINEFILE,"w");
                            for (w=0;w<onlcount;w++)
                            {
                                fprintf(onlfile,"%s %s %d\n",onluser[w],onlpass[w],onlsock[w]);
                            }
                            fclose(onlfile);
                        }
    
                            
                        printf("============================================\n");

                    }
                }
            }
        }
    }
    return 0;
}
