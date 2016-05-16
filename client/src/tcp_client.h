#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "serialize.h"
#include "comm.h"
#include "error.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1B[37m"
#define RESET_COLOR "\033[0m"

void handle_tcp_packets();

void write_login(char * username, char * password, char color);

void write_register(char * username, char * password);

void write_delete();

void write_talk(char * mensaje);
void handle_talk();

void write_change_color(BYTE color);

void write_change_pw(char * new_password);

void write_kick(char * username, char * reason);

void write_ban(char * username, char * reason);

void write_disconnect();
void handle_disconnect();

void write_check_logs(char * from, char * to);

void write_get_online_users();


#endif
