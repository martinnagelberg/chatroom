#include "database.h"
#include "logging.h"

//AGREGAR SERIALIZE CHAR PARA PACKET ID

int callback(void* NotUsed, int argc, char** argv, char** column_name);
int login_callback(void* user_login_info, int argc, char** argv, char** column_name);
int get_chatlog_callback(void* chatlog_ptr, int argc, char** argv, char** column_name);

// Main para testear
/*int main(int argc, char const *argv[])
{
    Login_info log_info;

    if (is_db_initiated() != QUERY_OK) {
        printf("Base de datos no encontrada. Creando nueva.\n");
        db_create();
    }

    if (argc == 2) { //hago que registre y se logee con el mismo usuario y contraseña, paja escribir las 2 por separado.
        char* user_pass = malloc(sizeof(char) * strlen(argv[1]));
        strcpy(user_pass, argv[1]);
        printf("%d\n", register_user(user_pass, user_pass));
        if (login(user_pass, user_pass, &log_info) == LOGIN_STATUS_SUCCESS)
            printf("Loguie bien.\n");
        else
            printf("Loguie mal.\n");
    }
    else
        fprintf(stderr, "Just 1 argument expected\n");

    return 0;
}*/

/**
 * Callback generico para la ejecucion de comandos SQL
 */
int callback(void* NotUsed, int argc, char** argv, char** column_name) {
    int i;
    for (i = 0 ; i < argc ; i++) {
        printf("%s = %s\n", column_name[i], argv[i] ? argv[i] : "NULL");
    }
    putchar('\n');
    return 0;
}

/**
 * Creation of both tables: users and chatlogs
 */
int db_create() {
    sqlite3* db;
    int rc;
    char sql[576];
    char* errMsg = 0;

    printf("Trying to create new chatroom database.\n");

    printf("--> DB path: %s\n", DB_FILE);
    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        fprintf(stderr, "Can't open DB file: %s\n", sqlite3_errmsg(db));
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
        printf("--> Creating USERS and CHATLOG tables ... ");
        sprintf(sql, "CREATE TABLE USERS(" \
          "ID INTEGER PRIMARY KEY," \
          "USERNAME CHAR(30) NOT NULL UNIQUE," \
          "PASSWORD CHAR(30) NOT NULL," \
          "PRIVILEGES INT DEFAULT 0," \
          "DATE_CREATED DATETIME NOT NULL," \
          "LAST_LOGIN DATETIME," \
          "BANNED_FLAG INTEGER DEFAULT 0);"
          "CREATE TABLE CHATLOG(" \
          "ID INTEGER PRIMARY KEY," \
          "DATE_TIME DATETIME NOT NULL," \
          "USER_ID INTEGER NOT NULL," \
          "MESSAGE TEXT NOT NULL," \
          "FOREIGN KEY (USER_ID) REFERENCES USERS(ID));");

        sqlite3_extended_result_codes(db, 1);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Couldn't create your table. Error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
        else {
            fprintf(stdout, "OK\n");
            printf("--> Creating username admin ... ");
            sprintf(sql, "INSERT INTO USERS(USERNAME, PASSWORD, PRIVILEGES, DATE_CREATED) VALUES ('admin', 'admin', %d, datetime('now', 'localtime'));", USER_ADMIN);
            rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

            if (rc != SQLITE_OK) {
                fprintf(stderr, "Couldn't register your username. Error: %s\n", errMsg);
                sqlite3_free(errMsg);
            } else {
                fprintf(stdout, "OK\n");
                printf("Chatroom database created successfully.\n\n");
            }
        }
		sqlite3_close(db);
	}

    return rc;
}

/**
 * Check if DB exists and is initiated - 0 means YES, else means NO
 */
int is_db_initiated() {
    sqlite3* db;
    int rc;
    char sql[256];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "--> Can't open DB file: %s\n", sqlite3_errmsg(db));
        log_error(WARNING, "Could not open DB file\n");
    }
    else {
        sprintf(sql, "SELECT USERNAME FROM USERS WHERE ID = 1");

        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

        if (rc != SQLITE_OK) {
            //fprintf(stderr, "Couldn't get the chatlog. Error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
        else {
            sprintf(sql, "SELECT USER_ID FROM CHATLOG WHERE ID = 1");

            rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

            if (rc != SQLITE_OK) {
                //fprintf(stderr, "Couldn't get the chatlog. Error: %s\n", errMsg);
                sqlite3_free(errMsg);
            }
        }
    }

    sqlite3_close(db);

    return rc;
}

/**
 * Registro un nuevo usuario indicando privilegios en mi base de datos si no esta ya registrado
 * Los privilegios estan definidos en database.h
 */
int register_user(char* username, char* password) {
    sqlite3* db;
    int rc;
    char sql[160];
    char* errMsg = 0;

    if (strlen(username) > SIZE_USERNAME || strlen(password) > SIZE_PASSWORD)
        rc = ERROR_PARAM_SIZE;
    else {
        rc = sqlite3_open(DB_FILE, &db);

        if (rc) {
            log_error(ERROR, "Could not open DB file\n");
        }
        else {
            sprintf(sql, "INSERT INTO USERS(USERNAME, PASSWORD, DATE_CREATED) VALUES ('%s', '%s', datetime('now', 'localtime'));", username, password);

            sqlite3_extended_result_codes(db, 1);
            rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                log_error(ERROR, "Could not register that username\n");
                sqlite3_free(errMsg);
            }
        }

        sqlite3_close(db);

        switch (rc) {
            case SQLITE_ERROR:
                return ERROR_GENERIC;
            case SQLITE_CONSTRAINT_UNIQUE:
                return ERROR_USER_ALREADY_REGISTERED;
        }
    }

    return rc;
}

/**
 * Modifico los permisos de cierto usuario
 */
int update_privileges(char* username, char privilege) {
    sqlite3* db;
    int rc;
    char sql[128];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
		sprintf(sql, "UPDATE USERS SET PRIVILEGES = %d WHERE USERNAME = '%s';", (int)privilege, username);
		rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
		
		if (rc != SQLITE_OK) {
			log_error(ERROR, "Could not modify the privileges\n");
			sqlite3_free(errMsg);
		}
		sqlite3_close(db);
	}
    return rc;
}

/**
 * Seteo el flag de banneado para cierto usuario
 */
int set_user_banned(char* username, char flag) {
	sqlite3* db;
    int rc;
    char sql[96];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
		sprintf(sql, "UPDATE USERS SET BANNED_FLAG = %d WHERE USERNAME = '%s';", (int)flag, username);

		rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
		if (rc != SQLITE_OK) {
			log_error(ERROR, "Could not modify the privileges.\n");
			sqlite3_free(errMsg);
		}
		sqlite3_close(db);
	}

    return rc;
}

/**
 * Cambio la password de cierto usuario.
 */
int change_password(char* username, char* password) {
	sqlite3* db;
    int rc;
    char sql[128];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
		sprintf(sql, "UPDATE USERS SET PASSWORD = '%s' WHERE USERNAME = '%s';", password, username);

		rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
		if (rc != SQLITE_OK) {
			log_error(ERROR, "Could not modify the password.\n");
			sqlite3_free(errMsg);
		}
		sqlite3_close(db);
	}

    return rc;
}

/**
 * Elimino un usuario por su username.
 */
int delete_username(char * username) {
    sqlite3* db;
    int rc;
    char sql[64];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
		sprintf(sql, "DELETE FROM USERS WHERE USERNAME = '%s';", username);

		rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
		if (rc != SQLITE_OK) {
			log_error(ERROR, "Could not delete that username\n");
			sqlite3_free(errMsg);
		}
		sqlite3_close(db);
	}

    return rc;
}

/**
 * Busca chatlogs entre tales fechas
 * Las fechas tienen el formato YYYY-MM-DD HH:MM:SS
 * Si el formato es incorrecto SQLite me tira error y lo handleo
 */
int get_chatlog(char* from, char* to, char** chatlog) {
    sqlite3* db;
    int rc;
    char sql[256];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
		sprintf(sql, "SELECT DATE_TIME, USERNAME, MESSAGE " \
			"FROM USERS, CHATLOG " \
			"WHERE USERS.ID = CHATLOG.USER_ID AND DATE_TIME >= datetime('%s') AND DATE_TIME <= datetime('%s');", from, to);

		rc = sqlite3_exec(db, sql, get_chatlog_callback, (void*) chatlog, &errMsg);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't get the logs you wanted. Error: %s\nErr Code: %d\n", errMsg, rc);
			log_error(ERROR, "Could not get the chatlog from the DB\n");
			sqlite3_free(errMsg);
		}
		sqlite3_close(db);
	}
    return rc;
}

/**
 * Voy poniendo el resultado en el puntero a char que me pasaron y lo voy reallocando con cada tupla que devuelve.
 */
int get_chatlog_callback(void* chatlog_ptr, int argc, char** argv, char** column_name) {
    char** chatlog = (char**) chatlog_ptr;
    int current_size = strlen(*chatlog);

    *chatlog = realloc(*chatlog, (current_size + strlen(argv[2]) + 1) * sizeof(char));
    if (*chatlog == 0) {
        fprintf(stderr, "No pudo alocarse la memoria necesaria para el chatlog. Cancelando...\n");
        return 1;
    }

    strcat(*chatlog, argv[2]);
    strcat(*chatlog, "\n");

    return 0;
}

/**
 * Inserto nuevo mensaje en el chatlog. Esto puede ser tanto un mensaje de usuario como de sistema.
 */
int insert_chatlog(char * username, char * message) {

    sqlite3* db;
    int rc;
    char sql[160];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    }
    else {
		sprintf(sql, "INSERT INTO CHATLOG(USER_ID, DATE_TIME, MESSAGE) VALUES ((SELECT ID FROM USERS WHERE USERNAME = '%s'), datetime('now', 'localtime'), '%s');", username, message);

		rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
		if (rc != SQLITE_OK) {
			log_error(ERROR, "Could not log the chatlog into the DB\n");
			sqlite3_free(errMsg);
		}
		sqlite3_close(db);
	}
    return rc;
}

/**
 * Funcion de login
 */
int login(char* username, char* password, Login_info* login_info) {
    sqlite3* db;
    int rc;
    char sql[160];
    char* errMsg = 0;

    rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        log_error(ERROR, "Could not open DB file\n");
    } else {
        //Search for the user and pw in the DB
        sprintf(sql, "SELECT USERNAME, PASSWORD, PRIVILEGES, BANNED_FLAG " \
            "FROM USERS " \
            "WHERE USERNAME='%s' AND PASSWORD='%s';", username, password);

        strcpy(login_info->username, username);
        strcpy(login_info->password, password);
        login_info->login_status = LOGIN_STATUS_FAIL;

        rc = sqlite3_exec(db, sql, login_callback, (void*)login_info, &errMsg);

        if (rc != SQLITE_OK) {
            log_error(ERROR, "Could not make your request at the time.\n");
            sqlite3_free(errMsg);
        }
        else if (login_info->login_status == LOGIN_STATUS_FAIL)
            rc = ERROR_USER_OR_PW_INCORRECT;
        else if (login_info->login_status == LOGIN_STATUS_BANNED)
            rc = ERROR_USER_BANNED;
        else {
            //Update the last time the user logged in in the DB
            sprintf(sql, "UPDATE USERS SET LAST_LOGIN = datetime('now', 'localtime') WHERE USERNAME = '%s';", login_info->username);

            if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
				log_error(WARNING, "Could not update the last lastime the user logged.\n");
                sqlite3_free(errMsg);
            }

        }
		sqlite3_close(db);
	}

    return rc;
}

/**
 * Callback ejecutado al intentar logear. Si retorna distinto a 0 sqlite3_exec devuelve error.
 */
int login_callback(void* user_login_info, int argc, char** argv, char** column_name) {
    Login_info* login_info = (Login_info*) user_login_info;

    if (atoi(argv[3]) == 1) {
        login_info -> login_status = LOGIN_STATUS_BANNED;
        return LOGIN_STATUS_BANNED;
    }
    login_info->privileges = *(argv[2]) - 48;
    login_info->login_status = LOGIN_STATUS_SUCCESS;

    return 0;
}
