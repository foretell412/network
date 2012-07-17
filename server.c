#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXQUESTION 10
#define MYPORT 1234
#define MAX_BUF 256
#define USERFILE "userfile.txt"
#define ONLINEFILE "online.txt"
#define LIBRARY "library.txt"

int gameover=0;

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
FILE * loadQuestion(char filename[20],char question[2000][50],char answer[2000][50],int * qcount){
    FILE *file=fopen(filename,"r");
    if (file==NULL){
        perror("Can not open file\n");
        exit(0);
    }
    int c=0;int i;
    while(!feof(file)){
        fscanf(file,"%s",question[c]);
        fscanf(file,"%s",answer[c]);
        c++;
        
        
    }
    
    *qcount=c-1;
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
    int store1=0;
    int store2=0;
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
    char invite[200][50];
    int invitecount=0;
    char userlist[20][100];
    char passlist[20][100];
    char onluser[20][100];
    char onlpass[20][100];
    int onlsock[100];
    int onlstate[100];
    char challenger[20];
    char question[2000][50];
    char question2[2000][50];
    char answer[2000][50];
    char answer2[2000][50];
    int x;int y;int exist;int z;int flag;int timeup=0;
    int gamestart=0;int u1,u2;
    int truesock;
    int rivalsock;
    int qcount=0;
    int questionid[100];
    char message[20];
    char rival[20];
    char ans[200];
    char me[200];
    int seq1=0;
    int seq2=0;
    int haveChallenge[100][100];
    for (i=0;i<100;i++)
        for (j=0;j<100;j++)
        {
            haveChallenge[i][j]=0;
        }
    for (i=0;i<100;i++){
        onlstate[i]=0;
    }
    //reset information in onlinefile
    onlfile=fopen(ONLINEFILE,"w");
    fclose(onlfile);
    //load list of users and their passwords
    loadUserList(USERFILE,userlist,passlist,&count);
    count--;
      
    loadOnlineList(ONLINEFILE,onluser,onlpass,onlsock,&onlcount);
    onlcount--;
    //load question library
    loadQuestion(LIBRARY,question,answer,&qcount);

    for (i=0;i<qcount-1;i++){
        strcpy(question2[i],question[i]);
        strcpy(answer2[i],answer[i]);
        
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
    //select
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
                        }
                        printf("New connection from %s - Socket %d\n",inet_ntoa(theiraddr.sin_addr),newfd);
                    }
                //close socket     
                }else{ 
                    nbytes=recv(i,buf,sizeof(buf),0);
                    nowsock=i;
                    buf[nbytes]='\0';
                    printf("buf=%s\n",buf);
                                        
                    if(nbytes<=0){
                        //if socket closed
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
                        
                        //if sign up
                        if (strcmp(sub,"su")==0)
                        {
                            
                            x=2;
                            while(buf[x]!=' '){
                                user[x-2]=buf[x];
                                x++;
                            }
                            user[x-2]='\0';
                            //check user list to ensure 1user<->1username
                            x++;y=0;
                            while(x<=strlen(buf)){
                                passwd[y++]=buf[x++];
                            }
                            exist=0;
                            for (z=0;z<count;z++){
                                if (strcmp(user,userlist[z])==0){
                                    exist=1;
                                }
                            }
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
                            if (gamestart==0){
                                if (send(i,buf,strlen(buf),0)==-1){
                                    perror("Can not send message");
                            }
                            
                            }
                        }
                        //if login
                        if (strcmp(sub,"lg")==0){
                            x=2;
                            while(buf[x]!=' '){
                                user[x-2]=buf[x];
                                x++;
                            }
                            user[x-2]='\0';
                                                        
                            x++;y=0;
                            while(x<=strlen(buf)){
                                passwd[y++]=buf[x++];
                            }
                            //check password
                            int truepass=0;
                            
                            for (z=0;z<count;z++){
                                if ((strcmp(user,userlist[z])==0)&&(strcmp(passwd,passlist[z])==0)){
                                    truepass=1;
                                }
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
                            if (gamestart==0){
                                if (send(i,buf,strlen(buf),0)==-1){
                                    perror("Can not send message");
                                }  
                            }
                                                        
                        }
                        //display online player
                        if (strcmp(sub,"ol")==0){
                            printf("online people=%d\n",onlcount);
                            strcpy(buf,"");
                            for (w=0;w<onlcount;w++){
                                strcat(buf,onluser[w]);
                                printf("%s %d\n",onluser[w],onlsock[w]);
                                strcat(buf,"\n");
                                
                            }
                            if (gamestart==0){
                                if (send(i,buf,strlen(buf),0)==-1){
                                    perror("Can not send message");
                                } 
                            }
                               
                        }
                        printf("buf=%s\n",buf);
                        
                        //challenge
                        if (strcmp(sub,"rq")==0){
                            
                            x=2;
                            while((buf[x]!=' ')&&(buf[x]!='\0')){
                                rival[x-2]=buf[x];
                                x++;
                            }
                            
                            rival[x-2]='\0';
                            //flag show whether the rival received a chanllenge message from another player or not
                            //flag==0 <=> no, flag!=0 <=> yes
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
                            
                            
                            if (flag==0){
                                strcpy(message,"cl");
                                haveChallenge[nowsock][rivalsock]=1;
                                strcat(message,challenger);
                                if (gamestart==0)
                                    if (send(rivalsock,message,strlen(message),0)==-1){
                                        perror("can not send challenge\n");
                                    }
                            }
                            
                            //decline if rival received challenge from another
                            else if (flag==1){
                                // ca cannot challenge
                                strcpy(message,"cc");
                                strcat(message,rival);
                                if (gamestart==0)
                                    if (send(nowsock,message,strlen(message),0)==-1){
                                        perror("can not send challenge\n");
                                    }
                            }
                                                        
                         
                        }
                        // accept challenge message
                        
                        if ((strcmp(sub,"ac")==0)){
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
                                if (strcmp(rival,onluser[z])==0){
                                    
                                    rivalsock=onlsock[z];
                                }
                            }
                            //if (haveChallenge[nowsock][rivalsock]==1)
                            if (1){
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
                            
    
                        } 
                        //decline challenge message
                        if ((strcmp(sub,"dc")==0)){
                            x=2;
                            while((buf[x]!=' ')&&(buf[x]!='\0')){
                                rival[x-2]=buf[x];
                                x++;
                            }
                            rival[x-2]='\0';
                            for (z=0;z<onlcount;z++){
                                if (strcmp(rival,onluser[z])==0){
                                    
                                    rivalsock=onlsock[z];
                                }
                                if (onlsock[z]==nowsock){
                                    strcpy(me,onluser[z]);
                                }
                                    
                            }
                            //cc can't access
                            strcpy(message,"cc");
                            strcat(message,me);
                            if (gamestart==0)
                                if (send(rivalsock,message,strlen(message),0)==-1){
                                    perror("can not send challenge\n");
                                }
                            haveChallenge[rivalsock][nowsock]=0;
                        }
                        if ((strcmp(sub,"as")==0)&&(gamestart==1)){
                            x=2;
                            while((buf[x]!=' ')&&(buf[x]!='\0')){
                                ans[x-2]=buf[x];
                                x++;
                            }
                            ans[x-2]='\0';

                            if (nowsock==u1){
                                if(strcmp(ans,answer[q1])==0){
                                    seq1++;
                                    q1++;

                                    if (seq1==3) {
                                        srand(time(NULL));
                                        int r=rand()%80+91;
                                        if (store2<q2+1) store2=q2+1;
                                        else store2++;
                                        strcpy(question2[store2],question2[r]);
                                        strcpy(answer2[store2],answer2[r]);

                                    }
                                    if (seq1==6) {
                                        srand(time(NULL));
                                        int r=rand()%80+181;
                                        if (store2<q2+1) store2=q2+1;
                                        else store2++;
                                        strcpy(question2[store2],question2[r]);
                                        strcpy(answer2[store2],answer2[r]);
                                        seq1=0;
                                        
                                    }
                                    if (q1>=MAXQUESTION){
                                        if (gamestart==1)
                                        {
                                            
                                            strcpy(message,"gowinner");
                                            send(u1,message,strlen(message),0);
                                            strcpy(message,"goloser");
                                            send(u2,message,strlen(message),0);
                                        }
                                        

                                    }
                                    else{
                                        if (gamestart==1)
                                        {
                                            strcpy(message,"qu");
                                            strcat(message,question[q1]);
                                            send(u1,message,strlen(message),0);
                                        }
                                        
                                    }
                                    
                                }
                                else{
                                    seq1=0;
                                }
                            }
                            if (nowsock==u2){
                                if (strcmp(ans,answer2[q2])==0){
                                    q2++;
                                    seq2++;
                                    if (seq2==3) {
                                        srand(time(NULL));
                                        int r=rand()%80+91;
                                        if (store1<q1+1) store1=q1+1;
                                        else store1++;
                                        strcpy(question[store1],question[r]);
                                        strcpy(answer[store1],answer[r]);
                                    }
                                    if (seq2==6) {
                                        srand(time(NULL));
                                        int r=rand()%80+181;
                                        if (store1<q1+1) store1=q1+1;
                                        else store1++;
                                        strcpy(question[store1],question[r]);
                                        strcpy(answer[store1],answer[r]);
                                        seq2=0;
                                    }
                                    if (q2>=MAXQUESTION){
                                        
                                        strcpy(message,"gowinner");
                                        send(u2,message,strlen(message),0);
                                        strcpy(message,"goloser");
                                        send(u1,message,strlen(message),0);
                                        
                                    }
                                    else{
                                        strcpy(message,"qu");
                                        strcat(message,question2[q2]);
                                        send(u2,message,strlen(message),0); 
                                    }
                                                                    
                                }
                                else {
                                    seq2=0;
                                }
                            }                          
                        }
                        if ((strcmp(sub,"so")==0)&&(gamestart!=1)){
                            for (w=0;w<onlcount;w++)
                            {
                                if (onlsock[w]==nowsock){
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
                                printf("%s %s %d\n",onluser[w],onlpass[w],onlsock[w]);

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
