#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644

struct message{
    long mtype;
    char mtext[200];
};

int main(void){
    int msgid;
    key_t key;
    struct message buf;

    if((key = ftok("server.c",'B')) == -1){
        perror("Error in ftok. Please check if the file exists");
        exit(1);
    }
    if((msgid = msgget(key, PERMS)) == -1){
        perror("Error in msgget. Message queue doesn't exist");
        exit(1);    
    }

    while(1){
        char choice;
        printf("Do you want the server to terminate? Press Y for Yes and N for No. ");
        scanf("%c",&choice);
        if(choice == 'Y' || choice=='y')break;
    }
    buf.mtype = 1;
    if(msgsnd(msgid,&buf,sizeof buf.mtext,0) == -1){
        perror("Error in msgsend. Unknown Error.");
        exit(1);
    }
    printf("Sent message to server to cleanup.\n");
   return 0;
}