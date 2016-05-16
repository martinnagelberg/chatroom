#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>  
#include "comm.h"
#include "error.h"

int _build_socket(void * address, struct sockaddr_in * s_address){

    connection_info * socket_info = (connection_info *) address;

    if (strcmp(socket_info->ip, "127.0.0.1")){
        s_address->sin_addr.s_addr = inet_addr(strdup(socket_info->ip));
    }else{
        s_address->sin_addr.s_addr = INADDR_ANY;

    }

    s_address->sin_family = AF_INET;
    s_address->sin_port = htons(socket_info->port);

    return 0;

}


int connect_to(void * address){

    int socket_fd;
    struct sockaddr_in sock;

    if ( (socket_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        return raise_error(ERR_RES_CREATION);
    } 

    _build_socket(address, &sock);

    if (connect(socket_fd, (struct sockaddr *)&sock, sizeof(sock)) == -1) {
        return raise_error(ERR_CON_REFUSED);
    }

    return socket_fd;

}

int disconnect(int connection_descriptor){

    close(connection_descriptor);

    return 0;

}


int send_data(int connection_descriptor, void * message, int bytes_to_write){

    int written_bytes;

    while ( (written_bytes = send(connection_descriptor, message, bytes_to_write, 0)) < bytes_to_write) {}

    return written_bytes;
}

int receive_data(int connection_descriptor, void * ret_buffer){

    int read_bytes = recv(connection_descriptor , ret_buffer , 512 , 0); //chunksize 512
    char * chars = (char *) ret_buffer;
    
    while ( chars[read_bytes - 1] != END_MESSAGE_SENTINEL) {
        read_bytes += recv(connection_descriptor , ret_buffer , 512 , 0);
    }  

    return read_bytes;

}
    
int listen_connections(void * address, main_handler handler, int* run_condition){

    int listener_socket;
    int new_socket_fd;
    struct sockaddr_in sock;
    struct sockaddr_in client;
    context_info context;

    context.run = run_condition;

    if ( (listener_socket = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        return raise_error(ERR_ADDRESS_IN_USE);        
    } 

     _build_socket(address, &sock);

     context.listener_descriptor = listener_socket;

    if( bind(listener_socket,(struct sockaddr *)&sock , sizeof(sock)) < 0)
       return raise_error(ERR_RES_CREATION);
    
    
    listen(listener_socket , 3);

    int c = sizeof(struct sockaddr_in);

    while (*run_condition)
    {
        
        new_socket_fd = accept(listener_socket, (struct sockaddr *)&client, (socklen_t*)&c);

        if (new_socket_fd < 0) {
            return raise_error(ERR_CON_REJECTED);
        }

        context.new_connection_descriptor = new_socket_fd;

        handler(&context);
    }

    disconnect(listener_socket);
     
    return 0;

}
