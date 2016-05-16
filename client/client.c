#include "comm.h"
#include "error.h"
#include "serialize.h"
#include "parser.h"
#include "tcp_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>

#define STDIN_BUFFER_SIZE 256
#define DEFAULT_COLOR 6

int client_connection_id;
int run;
fd_set fds;
int maxfd;
t_buffer * client_send_buffer;
t_buffer * client_recv_buffer;
connection_info server_info;
int logged;

void show_help();

//select en cliente.
//sacar los gccextras para que no tire warnings el clang. Del o:c
//errpres con rojo.

void init_client(char * ip, int port){

    strcpy(server_info.ip, ip);
    server_info.port = port;

    client_send_buffer = create_buffer();
    client_recv_buffer = create_buffer();

    run = 1;

}

void deinit_client(){

    delete_buffer(client_send_buffer);
    delete_buffer(client_recv_buffer);
    //destruir arg1 y arg2? free
}


int main(int argc , char *argv[])
{


    int r_bytes;
    char user_input[STDIN_BUFFER_SIZE];
    char* arg1 = NULL;
    char* arg2 = NULL;
	int cmd;

	
    init_client("127.0.0.1", 8888);

    maxfd = 0;
    client_connection_id = 0;
    
    while (run){

        FD_ZERO(&fds);

    	if (logged)
        	FD_SET(client_connection_id, &fds); //al establecer conexion tengo que hacerlo.
        
    
        FD_SET(0, &fds); //stdin
        select(maxfd+1, &fds, NULL, NULL, NULL);

        if (FD_ISSET(0, &fds)){
      
   			fgets(user_input, STDIN_BUFFER_SIZE, stdin);
   			
   			if (*user_input == '\n')
				continue;

   			
			cmd = parse_cmd(user_input, &arg1, &arg2);

            switch(cmd) {
				
				case CMD_CHAT:
					write_talk(user_input);
					break;
				case CMD_LOGIN:
					write_login(arg1, arg2, DEFAULT_COLOR);
					break;
				case CMD_REGISTER:
					write_register(arg1, arg2);
					break;
				case CMD_LOGOUT:
					write_disconnect();
					break;
				case CMD_CH_PW:
					write_change_pw(arg1);
					break;
				case CMD_CH_PRIVS:
					write_change_privileges(arg1, (unsigned char)(*arg2 - '0'));
					break;
				case CMD_CH_COLOR:
					write_change_color((unsigned char)(*arg1 - '0'));
					break;
				case CMD_DELETE_USER:
					write_delete();
					break;
				case CMD_KICK:
					write_kick(arg1, arg2);
					break;
				case CMD_BAN:
					write_ban(arg1, arg2);
					break;
				case CMD_GET_ONLINE_USERS:
					write_get_online_users();
					break;
				case CMD_HELP:
					show_help();
					break;
				case CMD_ERROR:
					fprintf(stderr, "Comando no reconocido. Escriba /help para ver la lista de comandos...\n");
					break;				
			}
			

			if (arg1 != NULL){
				free(arg1);
			}


			if (arg2 != NULL){
				free(arg2);
			}
			

			arg1 = arg2 = NULL;
        }
        

        
        if (logged && FD_ISSET(client_connection_id, &fds)){

  
            r_bytes = load_buffer(client_connection_id, client_recv_buffer);

            while (client_recv_buffer->pos + 1 < r_bytes){
                handle_tcp_packets();
                
                client_recv_buffer->pos+=1;
            }

            clean_buffer(client_recv_buffer);
        }
    }

    deinit_client();
     
    return 0;
}

void show_help() {
	printf("\nFunciones fuera del chat:\n");
	printf("--> /login usuario contraseña\n");
	printf("--> /register usuario contraseña\n\n");
	printf("Funciones dentro del chat:\n");
	printf("--> /change_password contraseña_nueva\n");
	printf("--> /change_color color\n");
	printf("--> /change_privileges usuario privilegios_nuevos\n");
	printf("--> /delete\n");
	printf("--> /get_online_users\n");
	printf("--> /kick usuario 'razon'\n");
	printf("--> /ban usuario 'razon'\n");
	printf("--> /logout\n\n");
}
