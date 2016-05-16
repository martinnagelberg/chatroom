#ifndef COM_H
#define COM_H


typedef struct
{
	char ip[20];
	int port;
}connection_info;

typedef struct
{
	int * run;
	int listener_descriptor;
	int new_connection_descriptor;
}context_info;

#define END_MESSAGE_SENTINEL '#'

typedef void (* main_handler) (context_info * context);

int connect_to(void * address);

int disconnect(int connection_descriptor);

int send_data(int connection_descriptor, void * message, int length);

int receive_data(int connection_descriptor, void * ret_buffer);

int listen_connections(void * address, main_handler handler, int* run_condition);


#endif