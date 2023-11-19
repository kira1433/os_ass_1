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
    int c_id;
    printf("Enter Client-ID:");
    scanf("%d",&c_id);

    struct message buf;
    int msgid;
    key_t key;
    if((key = ftok("server.c",'B')) == -1){
        perror("Error in ftok. Please check if the file exists");
        exit(1);
    }
    if((msgid = msgget(key, PERMS)) == -1){
        perror("Error in msgget. Message queue doesn't exist");
        exit(1);    
    };

    while(1){
        int choice;
        printf("1. Enter 1 to contact the Ping Server\
            \n2. Enter 2 to contact the File Search Server\
            \n3. Enter 3 to contact the File Word Count Server\
            \n4. Enter 4 if this Client wishes to exit\
            \n");
        scanf("%d",&choice);

        if(choice<0 || choice>4){
            printf("Invalid Choice.\n");
            continue;
        }

        buf.mtype = (1l<<31)*choice + c_id;
        if(choice == 1){
            strcpy(buf.mtext,"hi");
        }
        else if(choice == 2 || choice == 3){
            getchar();
            fgets(buf.mtext,sizeof buf.mtext,stdin);
            buf.mtext[strlen(buf.mtext)-1] = '\0';
        }
        else break;

        printf("Client : %s\n",buf.mtext);
        if(msgsnd(msgid,&buf,sizeof buf.mtext,0) == -1){
            perror("Error in msgsend. Unknown Error");
            exit(1);
        }
        if(msgrcv(msgid,&buf,sizeof buf.mtext, buf.mtype = (1l<<33) + (1l<<31)*choice + c_id ,0) == -1){
            perror("Error in msgrcv. Unknown Error");
            exit(1);
        }
        printf("Server : %s\n",buf.mtext);
    }
    
    return 0;
}