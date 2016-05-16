#include "tcp_server.h"

#define MAX_USERS 100
#define WELCOME_MSG "Servidor>> Bienvenido al chatroom."
#define SERVER_COLOR 1
#define ERROR_COLOR 0
#define DEFAULT_COLOR 6
#define AUX_BUFFER_SIZE 128
#define MAIN_BUFFER_SIZE 30


typedef enum _packet_id{ //comando para admin : kILL server
	LOGIN,               
	REGISTER_USER,
	DELETE_USER,
	TALK,
	CHANGE_COLOR,
	CHANGE_PW,
	KICK,
	BAN,
	DISCONNECT,
	CHECK_LOGS,
	USERS_ONLINE
} packet_id;

extern t_user * user_list[MAX_USERS];
extern int connected_users;

int name_to_index(char * name){

    for (int i = 0; i < connected_users; i++){

        if (strcmp(user_list[i]->name, name) == 0){
            return i;
        }
    }

    return -1;
}

int user_already_online(char * username){

	return (name_to_index(username) != -1);

}

void handle_tcp_packets(int user_index){

    BYTE p_id;

    read_byte(user_list[user_index]->recv_buffer, &p_id);

    switch (p_id){

        case LOGIN:
            handle_login(user_index);
            break;

        case REGISTER_USER:
        	handle_register(user_index);
        	break;

        case DELETE_USER:
        	handle_delete(user_index);

        case TALK:
        	handle_talk(user_index);
        	break;

        case CHANGE_COLOR:
        	handle_change_color(user_index);
        	break;

        case CHANGE_PW:
        	handle_change_pw(user_index);
        	break;

        case KICK:
        	handle_kick(user_index);
        	break;

        case BAN:
        	handle_ban(user_index);
        	break;

        case DISCONNECT:
        	handle_disconnect(user_index);
        	break;

        case CHECK_LOGS:
        	handle_check_logs(user_index);
        	break;

        case USERS_ONLINE:
        	handle_users_online(user_index);
        	break;
        
    }

}

void server_login(int user_index, char * username, char color, char privileges){

	user_list[user_index]->name = strdup(username); 
	user_list[user_index]->color = color;
	user_list[user_index]->privileges = privileges;
	user_list[user_index]->logged = 1;

	char aux_buffer[AUX_BUFFER_SIZE];
	sprintf(aux_buffer, "Servidor>> Se conectó %s a la sala.", username);

	for (int i = 0; i < connected_users; i++){
		write_talk(i, aux_buffer, SERVER_COLOR);
	}

	connected_users++;

	log_error(INFO, "User connected");

}

void handle_login(int user_index){

	char username [MAIN_BUFFER_SIZE], password [MAIN_BUFFER_SIZE];
	BYTE color;
	Login_info log_info;

	read_string(user_list[user_index]->recv_buffer, username); 
	read_string(user_list[user_index]->recv_buffer, password); 
	read_byte(user_list[user_index]->recv_buffer, &color);


	int ret_val = login(username, password, &log_info);

	if (ret_val != QUERY_OK){

		if(ret_val == ERROR_USER_OR_PW_INCORRECT) {
			write_talk(user_index, "Usuario o contraseña inválidos.", ERROR_COLOR);
			log_error(WARNING, "Usuario o contraseña inválidos.");
		}

		else if (ret_val == ERROR_USER_BANNED){
			write_talk(user_index, "Usuario baneado.", ERROR_COLOR);
			log_error(WARNING, "Usuario baneado.");
		}

		write_disconnect(user_index);
		log_error(INFO, "Usuario desconectado.");
		return;
	}

	char aux_buff [AUX_BUFFER_SIZE];

	if (log_info.privileges == USER_MOD){
		sprintf(aux_buff,"+%s", username);
	}else if (log_info.privileges == USER_ADMIN) {
		sprintf(aux_buff,"*%s", username);
	}else{
		sprintf(aux_buff,"%s", username);
	}

	if ((user_already_online(aux_buff))){
		write_talk(user_index, "Ese usuario ya está logeado.", ERROR_COLOR);
		log_error(WARNING, "Usuario ya logueado.");
		write_disconnect(user_index);
		log_error(INFO, "Usuario desconectado.");
		return;
	}

	server_login(user_index, aux_buff, color, log_info.privileges);

	write_talk(user_index, WELCOME_MSG, SERVER_COLOR);


}

void handle_register(int user_index) {

	char username [MAIN_BUFFER_SIZE], password [MAIN_BUFFER_SIZE];

	read_string(user_list[user_index]->recv_buffer, username); 
	read_string(user_list[user_index]->recv_buffer, password);

	if(register_user(username, password) == ERROR_USER_ALREADY_REGISTERED){
		write_talk(user_index, "Usuario ya existente.", ERROR_COLOR);
		log_error(WARNING, "Usuario ya existente.");
		write_disconnect(user_index);
		log_error(INFO, "Usuario desconectado.");
		return;
	}

	server_login(user_index, username, DEFAULT_COLOR, USER_NORMAL);


	write_talk(user_index, WELCOME_MSG, SERVER_COLOR);
}

void handle_delete(int user_index){

	delete_username(user_list[user_index]->name);
	write_talk(user_index, "Tu usuario fue borrado con éxito", SERVER_COLOR);
	log_error(INFO, "Usuario fue borrado con éxito.");
	write_disconnect(user_index);
	log_error(INFO, "Usuario desconectado.");

}

void handle_talk(int user_index){ //user index es el ejecutante. target a quien se lo mando.

	char message[AUX_BUFFER_SIZE], data[AUX_BUFFER_SIZE];
	int color = user_list[user_index]->color;

	read_string(user_list[user_index]->recv_buffer, data);

	sprintf(message, "%s>> %s", user_list[user_index]->name, data);

	insert_chatlog(user_list[user_index]->name, data); //guardo los logs

	for (int i = 0; i < connected_users; i++){
		write_talk(i, message, color);
	}

}

void write_talk(int target_index, char * message, int color){

	write_byte(user_list[target_index]->send_buffer, TALK);
	write_string(user_list[target_index]->send_buffer, message);
	write_byte(user_list[target_index]->send_buffer, color);

	flush_buffer(user_list[target_index]->connection_descriptor, user_list[target_index]->send_buffer);
	
}

void handle_change_color(int user_index){

	read_byte(user_list[user_index]->recv_buffer, &(user_list[user_index]->color));

}

void handle_change_pw(int user_index){

	char new_password [MAIN_BUFFER_SIZE];

	read_string(user_list[user_index]->recv_buffer, new_password); 

	change_password(user_list[user_index]->name, new_password);

	write_talk(user_index, "Contraseña cambiada satisfactoriamente.", SERVER_COLOR);
	log_error(INFO, "Contraseña cambiada satisfactoriamente.");

}

void handle_kick(int user_index){

	char username [MAIN_BUFFER_SIZE], reason [AUX_BUFFER_SIZE], aux_buff [AUX_BUFFER_SIZE * 2];

	read_string(user_list[user_index]->recv_buffer, username); 
	read_string(user_list[user_index]->recv_buffer, reason); 

	if (user_list[user_index]->privileges < USER_MOD){
		write_talk(user_index, "No tenés suficientes privilegios.", ERROR_COLOR);
		log_error(WARNING, "El usuario no tiene suficientes privilegios.");
		return;
	}

	sprintf(aux_buff, "Kickeado por: %s. Motivo: %s", user_list[user_index]->name, reason);

	int target_index = name_to_index(username);
	write_talk(target_index, aux_buff, SERVER_COLOR);
	handle_disconnect(target_index);

}

void handle_ban(int user_index){

	char username [MAIN_BUFFER_SIZE], reason [AUX_BUFFER_SIZE], aux_buff [AUX_BUFFER_SIZE * 2];

	read_string(user_list[user_index]->recv_buffer, username); 
	read_string(user_list[user_index]->recv_buffer, reason); 

	if (user_list[user_index]->privileges < USER_ADMIN){
		write_talk(user_index, "No tenés suficientes privilegios.", ERROR_COLOR);
		log_error(WARNING, "El usuario no tiene suficientes privilegios.");
		return;
	}

	sprintf(aux_buff, "Baneado por: %s. Motivo: %s", user_list[user_index]->name, reason);

	int target_index = name_to_index(username);
	
	set_user_banned(username, 1);
	write_talk(target_index, aux_buff, SERVER_COLOR);
	write_disconnect(target_index);
	log_error(INFO, "Usuario desconectado.");

}

// log_error(INFO, "User left");
void handle_disconnect(int user_index){

	write_talk(user_index, "Hasta la proxima...", SERVER_COLOR);
	write_disconnect(user_index);
	log_error(INFO, "Usuario desconectado.");

}

void write_disconnect(int user_index){

	write_byte(user_list[user_index]->send_buffer, DISCONNECT);
	flush_buffer(user_list[user_index]->connection_descriptor, user_list[user_index]->send_buffer);

	disconnect(user_list[user_index]->connection_descriptor);

	user_list[user_index]->connection_descriptor = -1;
	
	if (user_list[user_index]->logged){

		user_list[user_index]->logged = 0;

		connected_users--;

		char aux_buffer[75];
		sprintf(aux_buffer, "Servidor>> Se desconectó %s de la sala.", user_list[user_index]->name);

		for (int i = 0; i < connected_users; i++){
			write_talk(i, aux_buffer, SERVER_COLOR);
		}

		log_error(INFO, "User disconnected"); //cambiar nombre a esta pija
	}
	

}

void handle_check_logs(int user_index){

	char from [AUX_BUFFER_SIZE], to [AUX_BUFFER_SIZE];

	read_string(user_list[user_index]->recv_buffer, from); 
	read_string(user_list[user_index]->recv_buffer, to); 

}

void handle_users_online(int user_index){

	char * init_string = "Usuarios online:";
	char aux_buffer[(MAIN_BUFFER_SIZE * connected_users) + strlen(init_string)+1];
	
	strcpy(aux_buffer, init_string);

	for (int i=0; i < connected_users; i++){
		strcat(aux_buffer, "\n");
		strcat(aux_buffer, user_list[i]->name);
	}
	strcat(aux_buffer, "\n");


	printf("Resultado: %s\n", aux_buffer);

	write_talk(user_index, aux_buffer, SERVER_COLOR);

}



