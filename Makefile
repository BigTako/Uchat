OBJDIR = obj
SRCDIR = src
SERVER_DIR = server
CLIENT_DIR = client
SERVER = userver
CLIENT = uclient

SRC = $(SRCDIR)/*.c
OBJS = $(OBJDIR)/*.o

all: $(SERVER) $(CLIENT)

$(SERVER):
	make -sC $(SERVER_DIR)

$(CLIENT):
	make -sC $(CLIENT_DIR)

clean:
	rm -f $(OBJS)
	rm -df $(OBJDIR) 

uninstall:
	make -sC $(SERVER_DIR) $@
	make -sC $(CLIENT_DIR) $@
	make clean
	rm -f $(UCHAT)

reinstall:
	make uninstall
	make all
