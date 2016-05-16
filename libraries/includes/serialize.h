#ifndef SERIALIZE_H
#define SERIALIZE_H

//#include "comm.h"

typedef unsigned char BYTE;


typedef struct _t_buffer{ //uno para leer y otro para escribir

	BYTE * data;
	int pos;

}t_buffer;

/*
int write_int(int connection_descriptor, int message);
int write_string(int connection_descriptor, char * message);


int read_int(int connection_descriptor);
int read_string(int connection_descriptor, char * read_buffer);
*/

t_buffer * create_buffer();
void delete_buffer(t_buffer * buffer);

void write_byte(t_buffer * buffer , BYTE number);
void read_byte(t_buffer * buffer , BYTE * number);
void write_int(t_buffer * buffer , int number);
void write_string(t_buffer * buffer, char * string);
void read_int(t_buffer * buffer, int * number);
void read_string(t_buffer * buffer, char * string);
int flush_buffer(int connection_descriptor, t_buffer * buffer);
void clean_buffer(t_buffer * buffer);
int load_buffer(int connection_descriptor, t_buffer * buffer);



#endif



