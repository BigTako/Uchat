#include "../inc/header.h"

int main(int argc, char ** argv) 
{
    if (argc != 3)
    {
        printf("Usage: ./userver <server IP> <server port>\n");
        return 0;
    }
    /*SQL DATABASE TABLES INITIALISATION*/
    sqlite3 * db = NULL;
	sqlite3_open("database.db", &db);
	
    format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            username TEXT NOT NULL UNIQUE, \
                            password TEXT NOT NULL)", 1, USERS_TN);

    //creating table with messages
	format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            from_userid INTEGER NOT NULL, \
                            message_text TEXT NOT NULL, \
                            send_datetime TEXT NOT NULL, \
                            conversation_id TEXT NOT NULL, \
                            status TEXT NOT NULL DEFAULT 'unread')", 1, MESSAGES_TN);
  
  	//create table with group_members data
  	format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(user_id INTEGER NOT NULL, \
                            conversation_id TEXT NOT NULL, \
                            conversation_name TEXT NOT NULL DEFAULT 'group', \
                            joined_datetime TEXT NOT NULL, \
                            left_datetime TEXT NOT NULL)", 1, GROUP_MEMBERS_TN);

    format_and_execute(db, "INSERT INTO %s(username, password) VALUES('%s','%s')", 3, USERS_TN, "MAKS", "admin");

    /*GENERATION OF RSA KEYS*/
    EVP_PKEY * key = generate_key_pair();
    code privateKeyChar = PRIVKEY_to_str(key);
    code publicKeyChar = PUBKEY_to_str(key);
    EVP_PKEY_free(key);
    // data reveived from client
    /*
		<---CODES--->
		S@TEXT@TIME@CONVERSATION_ID - send message
		C@NAME@USERNAME1@USERNAME2@... - create new chat 
		F@USERNAME@CONVERSATION_ID - renew chat
		B@MESSAGE_ID
		D@MESSAGE_ID
		E@USERNAME@CONVERSATION_ID
	*/



    /*ВСТАВКА
	char buf[10000];
	int charcheck = sprintf(buf, "INSERT INTO messages VALUES(%d, %d, %s, %s, %s)", 1, 2, "'message'", "'2022'", "'asdfsdfsdfa'");
	execute_query(db, buf);
	printf("Query: %s\n", buf);*/

    /*ВИБІРКА
	int cols_count = 5;
	void *** table = get_db_data_vector(db, "SELECT * FROM messages WHERE message_id>5", cols_count);
	char * sheesh = NULL;
	int id = 0;
	for (int i = 0; table[i]; i++)
	{
		for (int j = 0; table[i][j]; j++)
		{
			sheesh = table[i][j];
			printf("%s", sheesh);
			if (table[i][j + 1]) printf("-");
		}	
		printf("\n");
	}
    delete_table(&table);
*/
    int welcomeSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (listen(welcomeSocket,5)==0)
    {
        printf("Listening\n");
    }
    else
    {
        printf("Error\n");
    }

    addr_size = sizeof serverStorage;

    pthread_mutex_t mutex_R;

    int cmdEXIT = 0;
    while (cmdEXIT == 0) {
        int Client = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
        pthread_t thread;
        t_thread_param* param = (t_thread_param*) malloc(sizeof(t_thread_param));

        param->cmdEXIT = &cmdEXIT;
        param->socket = Client;
        param->mutex_R = &mutex_R;
        param->db = db;
        int status = pthread_create(&thread, NULL, client_thread, param);
        if (status != 0) {
            printf("main error: can't create thread");
            exit(1);
        }
    }
    //int status_addr;

    
    //status = pthread_join(thread, (void**)&status_addr);
    //if (status != 0) {
    //    printf("main error: can't join thread");
    //    exit(1);
    //}

    free(privateKeyChar);
    free(publicKeyChar);
    sqlite3_close(db);
    return 0;
}

