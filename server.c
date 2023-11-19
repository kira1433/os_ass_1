#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define PERMS 0644

struct message{
    long mtype;
    char mtext[200];
};

int main(void){
    struct message buf;
    int msgid;
    key_t key;
    if((key = ftok("server.c",'B')) == -1){
        perror("Error in ftok. Please check if the file exists");
        exit(1);
    }
    if((msgid = msgget(key, PERMS | IPC_CREAT)) == -1){
        perror("Error in msgget. Message queue doesn't exist");
        exit(1);    
    }
    printf("Server Started with pid: %d\n",getpid());
    while(1){
        if(msgrcv(msgid,&buf,sizeof buf.mtext,-(1l<<33)+1,0) == -1){
            perror("Error in msgrcv. Unknown Error");
            exit(1);
        }
        int choice  = buf.mtype/(1l<<31);
        if(choice == 0){
            printf("Going to cleanup phase ,Waiting for child processes\n");
            while(wait(NULL) != -1);
            break;
        }
        pid_t pid = fork();
        if(pid == -1){
            perror("Main fork has failed.\n");
            exit(1);
        }
        if(pid == 0){
            int c_id = buf.mtype%(1l<<31);
            buf.mtype |= (1l<<33);
            printf("Child process with pid: %d serving client with c_id : %d for the task : %d!!\n",getpid(),c_id,choice);

            if(choice == 1){
                strcpy(buf.mtext,"hello");
            }
            else if(choice == 2){
                int fd[2];
                char pipe_buf[200] = {};

                if(pipe(fd) == -1){
                    perror("Pipe has failed.\n");
                    exit(1);
                }
                pid_t pid2 = fork();

                if(pid2 == -1){
                    perror("Child fork has failed.\n");
                    exit(1);
                }

                if(pid2 == 0){
                    close(fd[0]);
                    dup2(fd[1],1);
                    close(fd[1]);
                    execlp("ls","ls",NULL);
                    perror("execlp ls gave an error.\n");
                    return 0;
                }
                else{
                    wait(NULL);
                    close(fd[1]);
                    read(fd[0],pipe_buf,sizeof pipe_buf);
                    char* token = strtok(pipe_buf,"\n");
                    while(token != 0){
                        if(strcmp(token,buf.mtext) == 0){
                            strcpy(buf.mtext,"The file exists");
                            break;
                        }
                        token = strtok(NULL,"\n");
                    }
                    if(token == 0)strcpy(buf.mtext,"The file doesnt exists");
                    close(fd[0]);
                }
            }
            else{
                int fd[2];
                
                if(pipe(fd) == -1){
                    perror("Pipe has failed.\n");
                    exit(1);
                }

                pid_t pid2 = fork();

                if(pid2 == -1){
                    perror("Child fork has failed.\n");
                    exit(1);
                }

                if(pid2 == 0){
                    close(fd[0]);
                    dup2(fd[1],1);
                    close(fd[1]);
                    execlp("wc","wc","-w",buf.mtext,NULL);
                    perror("execlp wc gave an error.\n");
                    return 0;
                }
                else{
                    wait(NULL);
                    close(fd[1]);
                    read(fd[0],buf.mtext,sizeof buf.mtext);
                    strcpy(buf.mtext,strtok(buf.mtext,"\n"));
                    close(fd[0]);
                }
            }
            if(msgsnd(msgid,&buf,sizeof buf.mtext,0) == -1){
                perror("Error in msgsend. Unknown Error.");
                exit(1);
            }
            return 0;
        }
    }
    if(msgctl(msgid,IPC_RMID,NULL) == -1){
        perror("Error in msgsctl. Message queue doesn't exist.");
        exit(1);
    }
    printf("Cleanup Completed.");
    return 0;
}