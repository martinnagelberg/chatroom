#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_SIZE 256
#define LOGFILE_PATH "../logger/logs.txt"

struct msgbuf {
	long mtype;
	char message[MAX_SIZE];
};


int main(void)
{
    struct msgbuf buf;
    int msqid;
    key_t key = 123;

    if ((msqid = msgget(key, (IPC_CREAT | IPC_EXCL | 0666))) == -1) { 
        perror("Error while trying to create MQ");
        return -1;
    }
    
    while(1) { // corre siempre
        
        if (msgrcv(msqid, &buf, sizeof(buf.message), 0, 0) == -1) {
            perror("Error while trying to recieve from MQ");
            return -1;
        }

        FILE * fp = fopen(LOGFILE_PATH, "a");

	    if (fp == NULL){
            perror("Error while trying to create the log file");
            return -1;
	    }

        fprintf(fp,"%s\n", buf.message);
        fclose(fp);
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Error while trying to delete MQ\n");
        return -1;
    }


    return 0;
}