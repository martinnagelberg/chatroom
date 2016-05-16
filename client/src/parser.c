#include "parser.h"

static char* all_cmds[] = {"login", "register", "logout", "change_password", "change_privileges", "change_color", "delete", "kick", "ban"};
static int cmds_amount = 9;

void del_spaces(char* str);
void replace_char(char* str, char c1, char c2);

/**
 * Returns de command code and the arguments 1 and 2 initialized if needed
 */
int parse_cmd(char* msg, char** arg1, char** arg2) {
	
	int i = 0;
	int msg_length;
	int cmd_length;
	int arg1_length = 0;
	int arg2_length = 0;
	
	del_spaces(msg);
	
	if(*msg != '/') {	//Si no empieza con una barra es que estan chateando (no es comando)
		return CMD_CHAT;
	}
	
	while(msg[i] != ' ' && msg[i] != '\0')
		i++;
	
	msg_length = strlen(msg);
	msg[msg_length - 1] = '\0';		//saco el \n que viene al final porque sino jode.
	
	replace_char(msg, ' ', '\0');
	cmd_length = strlen(msg);
		
	if (msg_length - cmd_length - 1 > 0) {	//Si queda algo despues del comando
		arg1_length = strlen(msg + cmd_length + 1);
		*arg1 = malloc(sizeof(char) * arg1_length);
		strcpy(*arg1, msg + cmd_length + 1);
		if (msg_length - cmd_length - arg1_length - 2 > 0) { 	//Si hay un 2do arg
			arg2_length = strlen(msg + arg1_length + 2);
			*arg2 = malloc(sizeof(char) * arg2_length);
			strcpy(*arg2, msg + cmd_length + arg1_length + 2);
		}
	}
	
	for (int j = 0; j < cmds_amount; j++)
		if (strcmp(all_cmds[j], msg+1) == 0){
			msg_length = msg_length - cmd_length - 1;
			switch (j) {
				case 0: 	//login username password
					if (arg2_length == 0) {
						fprintf(stderr, FAILED_LOGIN_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_LOGIN;
					
				case 1:		//register username password
					if (arg2_length == 0) {
						fprintf(stderr, FAILED_REGISTER_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_REGISTER;
					
				case 2:		//logout
					if (arg1_length != 0) {
						fprintf(stderr, FAILED_LOGOUT_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_LOGOUT;
					
				case 3:		//change_password old_pass new_pass
					if (arg2_length == 0) {
						fprintf(stderr, FAILED_CHANGE_PASSWORD_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_CH_PW;
					
				case 4:		//change_privileges username new_priv
					if (arg2_length == 0) {
						fprintf(stderr, FAILED_CHANGE_PRIVILEGES_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_CH_PRIVS;
					
				case 5:		//change_color color
					if (arg1_length == 0 || arg2_length != 0) {
						fprintf(stderr, FAILED_CHANGE_COLOR_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_CH_COLOR;
					
				case 6:		//delete username
					if (arg1_length == 0) {
						fprintf(stderr, FAILED_DELETE_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_DELETE_USER;
					
				case 7:		//kick username reason
					if (arg2_length == 0) {
						fprintf(stderr, FAILED_KICK_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_KICK;
					
				case 8:		//ban username reason
					if (arg2_length == 0) {
						fprintf(stderr, FAILED_BAN_MSG);
						j = cmds_amount;
						break;
					}
					return CMD_BAN;
			}
		}
		
	return CMD_ERROR;
}

/**
 * Borra espacios extra que pueda haber en un string (y todos los primeros)
 */
void del_spaces(char* str){
	
	char* str2 = str;
	
	while (*str == ' ')	//hago que str apunte al primer char que no sea un espacio
		str++;

	str--; //fix porque despues lo aumento en el while. #AhListoQueVillero
	
	while (*str++ != 0){
		if (*str != ' ')
			*str2++ = *str;
		else{
			if (*(str+1) == ' ')
				continue;
			else
				*str2++ = *str;
			}
	}
}

/**
 * Reemplaza el char c1 por c2 en todo un string.
 */
 void replace_char(char* str, char c1, char c2) {
	 while (*str != '\0') {
		if (*str == c1)
			*str = c2;
		str++;
	}
 }



