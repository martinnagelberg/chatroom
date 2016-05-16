#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "serialize.h"
#include "comm.h"
#include "user.h"
#include <string.h>
#include <stdio.h>
#include "database.h"
#include "logging.h"


void handle_tcp_packets(int user_index);

void handle_login(int user_index);

void handle_register(int user_index);

void handle_delete(int user_index);

void handle_talk(int user_index);
void write_talk(int target_index, char * message, int color);

void handle_change_color(int user_index);

void handle_change_pw(int user_index);

void handle_kick(int user_index);

void handle_ban(int user_index);

void handle_disconnect(int user_index);
void write_disconnect(int target_index);

void handle_check_logs(int user_index);

void handle_users_online(int user_index);


#endif