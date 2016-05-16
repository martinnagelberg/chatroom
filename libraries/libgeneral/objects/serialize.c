#include "serialize.h"
#include "comm.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 512


t_buffer * create_buffer(){

	t_buffer * ret_buffer = (t_buffer *) malloc(sizeof(t_buffer));

	if (ret_buffer == NULL)
		return NULL;

	ret_buffer->pos = 0;
	ret_buffer->data = (BYTE *) malloc(sizeof(BYTE) * BUFFER_SIZE);

	if (ret_buffer->data == NULL){
		free(ret_buffer);
		return NULL;
	}

	bzero(ret_buffer->data, BUFFER_SIZE);
	return ret_buffer;

}
void delete_buffer(t_buffer * buffer){

	free(buffer->data);
	free(buffer);

}

void write_byte(t_buffer * buffer , BYTE number){

	memcpy(buffer->data + buffer->pos, &number, sizeof(int8_t));
	buffer->pos += sizeof(int8_t);
	
}

void read_byte(t_buffer * buffer , BYTE * number){

	memcpy(number, buffer->data + buffer->pos, sizeof(int8_t));
	buffer->pos+= sizeof(int8_t);

}

void write_int(t_buffer * buffer , int number){

	memcpy(buffer->data + buffer->pos, (char *) &number, sizeof(int32_t));
	buffer->pos += sizeof(int32_t);
	
}

void write_string(t_buffer * buffer, char * string){

	int string_len = strlen(string) + 1;

	write_int(buffer, string_len);

	memcpy(buffer->data + buffer->pos, string, string_len);
	buffer->pos+= string_len;

}

void read_int(t_buffer * buffer, int * number){

	memcpy(number, buffer->data + buffer->pos, sizeof(int32_t));
	buffer->pos+= sizeof(int32_t);

}

void read_string(t_buffer * buffer, char * string){

	int string_len;

	read_int(buffer, &string_len);
	memcpy(string, buffer->data + buffer->pos, string_len);
	buffer->pos+= string_len;

}

int flush_buffer(int connection_descriptor, t_buffer * buffer){

	buffer->data[buffer->pos] = END_MESSAGE_SENTINEL;
	buffer->pos+=1;
	int w_bytes = send_data(connection_descriptor, (void *) buffer->data, buffer->pos);
	clean_buffer(buffer);

	return w_bytes;

}

void clean_buffer(t_buffer * buffer){


	bzero(buffer->data, buffer->pos);
	buffer->pos = 0;
	

}

int load_buffer(int connection_descriptor, t_buffer * buffer){

	
	return receive_data(connection_descriptor, buffer->data);


}




