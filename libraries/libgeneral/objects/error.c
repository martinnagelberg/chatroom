#include "error.h"
#include "string.h"
#include "stdio.h"

#define RED "\033[31m"
#define RESET "\033[0m"



int last_error;

char * error_description[5] ={
	"Could not create resource.",
	"Connection refused.",
	"Connection timeout.",
	"Connection rejected.",
	"Address already in use."
}; //sumar uno al last error

int raise_error(int error_number){

	last_error = error_number - 1;
	return -1 * error_number;

}

char * str_error(int error_number){

	return error_description[error_number * -1];


}

void print_error(){

	fprintf(stderr, RED "%s\n" RESET, error_description[last_error]);

}