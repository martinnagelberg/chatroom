#ifndef ERROR_H
#define ERROR_H

#define ERR_RES_CREATION 1
#define ERR_CON_REFUSED 2
#define ERR_CON_TIMEOUT 3
#define ERR_CON_REJECTED 4
#define ERR_ADDRESS_IN_USE 5

int raise_error(int err_number);
char * str_error(int err_number);
void print_error();

#endif