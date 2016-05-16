#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FAILED_LOGIN_MSG "Command syntax error. Correct format is: /login username password\n"
#define FAILED_REGISTER_MSG "Command syntax error. Correct format is: /register username password\n"
#define FAILED_DELETE_MSG "Command syntax error. Correct format is: /delete username\n"
#define FAILED_CHANGE_COLOR_MSG "Command syntax error. Correct format is: /change_color color (0 to 6)\n"
#define FAILED_CHANGE_PASSWORD_MSG "Command syntax error. Correct format is: /change_password new_password\n"
#define FAILED_CHANGE_PRIVILEGES_MSG "Command syntax error. Correct format is: /change_privileges username new_privileges\n"
#define FAILED_KICK_MSG "Command syntax error. Correct format is: /kick username 'reason'\n"
#define FAILED_BAN_MSG "Command syntax error. Correct format is: /ban username 'reason'\n"
#define FAILED_LOGOUT_MSG "Command syntax error. Correct format is: /logout\n"
#define FAILED_GET_ONLINE_USERS "Command syntax error. Correct format is: /get_online:users\n"

#define CMD_ERROR 66
#define CMD_CHAT 0
#define CMD_LOGIN 1
#define CMD_REGISTER 2
#define CMD_CH_PW 3
#define CMD_CH_PRIVS 4
#define CMD_CH_COLOR 5
#define CMD_DELETE_USER 6
#define CMD_KICK 7
#define CMD_BAN 8
#define CMD_LOGOUT 9
#define CMD_GET_ONLINE_USERS 10

int parse_cmd(char* msg, char** arg1, char** arg2);


#endif
