#include "user.h"
#include <stdlib.h>


t_user * create_user(int connection_fd){

    t_user * ret_user = (t_user *) malloc(sizeof(t_user));

    if (ret_user == NULL)
        return NULL;

    ret_user->connection_descriptor = connection_fd;
    ret_user-> name = NULL;
    ret_user->color = 0;
    ret_user->privileges = 0;
    ret_user->logged = 0;

    ret_user->recv_buffer = create_buffer();

    if (ret_user->recv_buffer == NULL){
        free(ret_user);
        return NULL;
    }

    ret_user->send_buffer = create_buffer();

    if (ret_user->send_buffer == NULL){
        free(ret_user->recv_buffer);
        free(ret_user);
        return NULL;
    }

    return ret_user;

}


void add_user(t_user ** user_list, t_user * user, int place){

	user_list[place] = user;

}

void delete_user(t_user ** user_list, int user_index){

	t_user * user = user_list[user_index];

	user_list[user_index] = NULL;

	if (user->name != NULL)
		free(user->name);
	
	free(user->recv_buffer);
	free(user->send_buffer);
	free(user);

}

