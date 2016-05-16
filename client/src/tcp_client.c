#include "tcp_client.h"
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


extern int client_connection_id;
extern int run;
extern fd_set fds;
extern int maxfd;
extern t_buffer * client_send_buffer;
extern t_buffer * client_recv_buffer;
extern connection_info server_info;

typedef enum _packet_id{ 
 LOGIN,               
 REGISTER_USER,
 DELETE_USER,
 TALK,
 CHANGE_COLOR,
 CHANGE_PW,
 KICK,
 BAN,
 DISCONNECT,
 CHECK_LOGS
} packet_id;

char *c_colors[] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE};


void handle_tcp_packets(){

    BYTE p_id;

    read_byte(client_recv_buffer, &p_id);

    switch (p_id){

        case TALK:
            handle_talk();
            break;


        case DISCONNECT:
            handle_disconnect();
            break;

    }


}


void write_login(char * username, char * password, char color){
	
    if (client_connection_id > 0){
      printf(RED "Ya estás logeado.\n" RESET_COLOR);
      return;
    }

    client_connection_id = connect_to((void*)&server_info);

    if (client_connection_id <= 0){
        print_error();
        return;
    }

    maxfd = client_connection_id;
    FD_SET(client_connection_id, &fds);

    write_byte(client_send_buffer, LOGIN);
    write_string(client_send_buffer, username);
    write_string(client_send_buffer, password);
    write_byte(client_send_buffer, color);

    flush_buffer(client_connection_id, client_send_buffer);

}

void write_register(char * username, char * password){
	
   if (client_connection_id > 0){
    printf(RED "Tenés que desconectarte de tu usuario actual para poder crear otro.\n" RESET_COLOR);
    return;
   }

   client_connection_id = connect_to((void*)&server_info);

   if (client_connection_id < 0){
       print_error();
          return;
    }


    write_byte(client_send_buffer, REGISTER_USER);
    write_string(client_send_buffer, username);
    write_string(client_send_buffer, password);

    flush_buffer(client_connection_id, client_send_buffer);

}

void write_delete(){

  if (client_connection_id <= 0){
    printf(RED "%s\n" RESET_COLOR, "No estás conectado.");
    return;
   }

    write_byte(client_send_buffer, DELETE_USER);

    flush_buffer(client_connection_id, client_send_buffer);


}

void write_talk(char * mensaje){
	
   if (client_connection_id <= 0){
    printf(RED "%s\n" RESET_COLOR, "No estás conectado.");
    return;
   }

   write_byte(client_send_buffer, TALK);
   write_string(client_send_buffer, mensaje);

   flush_buffer(client_connection_id, client_send_buffer);

}

void handle_talk(){

   char mensaje[75];
   BYTE color;

   read_string(client_recv_buffer, mensaje);
   read_byte(client_recv_buffer, &color);

   printf("%s%s%s\n", c_colors[color], mensaje, RESET_COLOR); //colores y eso

}

void write_change_color(int color){

   if (client_connection_id <= 0){
    printf(RED "No estás conectado.\n" RESET_COLOR);
    return;
   }

   write_byte(client_send_buffer, CHANGE_COLOR);
   write_byte(client_send_buffer, color);

   flush_buffer(client_connection_id, client_send_buffer);

}

void write_change_pw(char * new_password){
	
    if (client_connection_id <= 0){
      printf(RED "No estás conectado.\n" RESET_COLOR);
      return;
    }


   write_byte(client_send_buffer, CHANGE_PW);
   write_string(client_send_buffer, new_password);


   flush_buffer(client_connection_id, client_send_buffer);


}

void write_kick(char * username, char * reason){
	
   if (client_connection_id <= 0){
    printf(RED "No estás conectado.\n" RESET_COLOR);
    return;
   }

   write_byte(client_send_buffer, KICK);
   write_string(client_send_buffer, username);
   write_string(client_send_buffer, reason);

   flush_buffer(client_connection_id, client_send_buffer);

}

void write_ban(char * username, char * reason){
	
   if (client_connection_id <= 0){
    printf(RED "No estás conectado.\n" RESET_COLOR);
    return;
   }

   write_byte(client_send_buffer, BAN);
   write_string(client_send_buffer, username);
   write_string(client_send_buffer, reason);

   flush_buffer(client_connection_id, client_send_buffer);

}

void write_disconnect(){

   if (client_connection_id <= 0){
    printf(RED "No estás conectado.\n" RESET_COLOR);
    return;
   }

   write_byte(client_send_buffer, DISCONNECT);

   flush_buffer(client_connection_id, client_send_buffer);

}

void handle_disconnect(){


   client_connection_id = 0;
   //clean_buffer(client_send_buffer);
   //clean_buffer(client_recv_buffer);


}

void write_check_logs(char * from, char * to){


   if (client_connection_id <= 0){
    printf(RED "No estás conectado.\n" RESET_COLOR);
    return;
   }

   write_byte(client_send_buffer, CHECK_LOGS);
  write_string(client_send_buffer, from);
   write_string(client_send_buffer, to);

   flush_buffer(client_connection_id, client_send_buffer);

}
