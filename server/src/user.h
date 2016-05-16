#ifndef USER_H
#define USER_H

#include "serialize.h"

typedef struct _t_user{
    char * name;
    int connection_descriptor;
    BYTE color;
    BYTE privileges;
    BYTE logged;
    t_buffer * recv_buffer;
    t_buffer * send_buffer;
}t_user;

t_user * create_user(int connection_fd);

void add_user(t_user ** user_list, t_user * user, int place);

void delete_user(t_user ** user_list, int user_index);


#endif