#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "comm.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

#define MAX_CONNECTIONS 128
#define MIN_CONN_FD 4

typedef struct _fifo_connection_data{

    int pipe_fd;
    int file_desc_r;
    int file_desc_w;
    char * pipe_address; //Para hacer el unlink mas facil
    char listening; //Para invertir canales de lectura y escritura 

} fifo_handler;

fifo_handler * connections_list[MAX_CONNECTIONS];
int connections_number;


int _fd_to_index(int fd){

    for (int i = 0; i <= MAX_CONNECTIONS; i++)
        if (connections_list[i]->pipe_fd == fd) return i;

    return -1;

}

int _get_free_index(){

    for(int i = MIN_CONN_FD; i <= connections_number + MIN_CONN_FD; i++)
        if (connections_list[i] == NULL) return i;

    return -1;

}

int _create_fifos(char * address){

    char aux_buffer[20];


    sprintf(aux_buffer, "%s_%s", address, "w");
    if (mkfifo(aux_buffer, 0666) == -1) {
        return raise_error(ERR_RES_CREATION);
    }


    sprintf(aux_buffer, "%s_%s", address, "r");
     if (mkfifo(aux_buffer, 0666) == -1) {
        return raise_error(ERR_RES_CREATION);
    }

    return 0;

}

void _add_fifo(fifo_handler * fh){

    connections_list[fh->pipe_fd] = fh;
    connections_number++;

}


int _open_fifos(fifo_handler * fh, char * address, char listening){

    char aux_buffer[20];
    int fdr, fdw;

    if (listening == 0){


        sprintf(aux_buffer, "%s_%s", address, "r");
        if ( (fdw = open( aux_buffer, O_WRONLY)) < 0)  
            return raise_error(ERR_RES_CREATION);

        sprintf(aux_buffer, "%s_%s", address, "w");
        if ( (fdr = open( aux_buffer, O_RDONLY)) < 0)
            return raise_error(ERR_RES_CREATION);

    }else{

        sprintf(aux_buffer, "%s_%s", address, "r");
        if ( (fdr = open( aux_buffer, O_RDONLY)) < 0)  
            return raise_error(ERR_RES_CREATION);
        
        sprintf(aux_buffer, "%s_%s", address, "w");
        if ( (fdw = open( aux_buffer, O_WRONLY)) < 0)
           	return raise_error(ERR_RES_CREATION);

        
    }

    fh->file_desc_w = fdw;
    fh->file_desc_r = fdr;

    fh->pipe_fd = fdr; //para que el fd sirva en el select. get free index no se usa y el define min sockets tampoco.

    return 0;

}

fifo_handler * _create_fifo_handler(char * address){

    fifo_handler * ret_fh = (fifo_handler *) malloc(sizeof(fifo_handler));
    

    ret_fh->pipe_fd = _get_free_index();
    ret_fh->pipe_address = strdup(address);

    if (ret_fh->pipe_address == NULL){
    	free(ret_fh); 
    	return NULL;
    }


    return ret_fh;

}

void _delete_fifo(int pipe_fd){ //que le paso? el pipefd, el pipe, el address?

    fifo_handler * del_fh = connections_list[_fd_to_index(pipe_fd)];
    char aux_buffer[20];

    close(del_fh->file_desc_r);
    close(del_fh->file_desc_w);

    sprintf(aux_buffer, "%s_%s", del_fh->pipe_address, "r"); 
    unlink(aux_buffer);
    sprintf(aux_buffer, "%s_%s", del_fh->pipe_address, "w"); 
    unlink(aux_buffer);

    free(del_fh->pipe_address);
    free(del_fh);
    connections_list[pipe_fd] = NULL;
    connections_number--;

}


int connect_to(void * address){ 

    char connection_string[20], receive_buffer[20], aux_buffer[40];
    int fd;
    int pid = getpid();

    connection_info * socket_info = (connection_info *) address; 

    sprintf(aux_buffer, "/tmp/%s_%s", socket_info->ip, "r"); 

    if ( (fd = open( (char *) aux_buffer, O_WRONLY)) < 0) 
        return raise_error(ERR_CON_REFUSED);

    sprintf(connection_string, "%d", pid);

    write(fd, connection_string, strlen((char *) connection_string)+1);
    close(fd);

    sprintf(aux_buffer, "/tmp/%s_%s", socket_info->ip, "w"); 
    
    if ( (fd = open(aux_buffer, O_RDONLY)) < 0)
        return raise_error(ERR_CON_TIMEOUT); //Timeout?
    

    read(fd , receive_buffer , 20); //SI NO ES UN OK, ES UN REJECTED
    
    sprintf(aux_buffer, "/tmp/fifo_%d", pid); 
    fifo_handler * fh = _create_fifo_handler(aux_buffer);
    _open_fifos(fh, aux_buffer, 0);
    _add_fifo(fh);
   
    close(fd);

    return fh->pipe_fd;

}

int disconnect(int connection_descriptor){

    _delete_fifo(connection_descriptor);

    return 0;

}

int send_data(int connection_descriptor, void * message, int bytes_to_write){ //deberia ir el real connection descriptor, el del open. Una funcion que te haga open, otra close.

    int written_bytes;
   
    while ( (written_bytes = write(connections_list[connection_descriptor]->file_desc_w, message, bytes_to_write)) < bytes_to_write) {}


    return written_bytes; // will be 0 if no bytes are written
}

int receive_data(int connection_descriptor, void * ret_buffer){


    int read_bytes = read(connections_list[connection_descriptor]->file_desc_r , ret_buffer , 512);
    char * chars = (char *) ret_buffer;

    while (chars[read_bytes - 1] != END_MESSAGE_SENTINEL) {

        read_bytes += read(connections_list[connection_descriptor]->file_desc_r , ret_buffer , 512);
    }

    return read_bytes;
}


int _accept_connection(fifo_handler *listener, char *client_id){

    char aux_buffer[20];
    fifo_handler * accepted_fifo;

    sprintf(aux_buffer, "/tmp/fifo_%s", client_id);
    accepted_fifo = _create_fifo_handler(aux_buffer);
    _create_fifos(aux_buffer);
    write(connections_list[listener->pipe_fd]->file_desc_w, "OK", 3);
    _open_fifos(accepted_fifo, aux_buffer, 1);
    _add_fifo(accepted_fifo);
    

    return accepted_fifo->pipe_fd;

}

// falta usar el handler
int listen_connections(void * address, main_handler handler, int* run_condition){ //char condition y ponerlo en lugar del 1.
    
    fifo_handler * listener_fifo;
    int new_connection_descriptor;
    char aux_buffer[20], aux_buff[40];
    context_info context;

    connection_info * socket_info = (connection_info *) address; 

    context.run = run_condition;

    sprintf(aux_buff, "/tmp/%s", socket_info->ip);
    listener_fifo = _create_fifo_handler(aux_buff);
    if (_create_fifos(aux_buff) != 0 ) raise_error(ERR_ADDRESS_IN_USE);
    _open_fifos(listener_fifo, aux_buff, 1);
    _add_fifo(listener_fifo);

    context.listener_descriptor = listener_fifo->pipe_fd;

    while (*run_condition){ //read, create, write accept, handler.

        if (read(connections_list[listener_fifo->pipe_fd]->file_desc_r, aux_buffer, 20) > 0){
           
           new_connection_descriptor = _accept_connection(listener_fifo, aux_buffer);

            if (new_connection_descriptor < 0) {
                return raise_error(ERR_CON_REJECTED);
            }

            context.new_connection_descriptor = new_connection_descriptor;
           
            handler(&context); 
        }

    }

    disconnect(listener_fifo->pipe_fd);
     
    return 0;
}
