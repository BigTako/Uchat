OBJDIR = obj
SRCDIR = src
SERVER_DIR = server
CLIENT_DIR = client
SERVER = userver
CLIENT = uclient

all: $(SERVER) $(CLIENT)

$(SERVER):
	make -sC $(SERVER_DIR)

$(CLIENT):
	make -sC $(CLIENT_DIR)

uninstall:
	make -sC $(SERVER_DIR) $@
	make -sC $(CLIENT_DIR) $@

reinstall:
	make uninstall
	make all