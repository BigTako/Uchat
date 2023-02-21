CC = clang
# CFLAGS = -std=gnu11 -pthread -g 
# -Wall -Wextra -Werror -Wpedantic
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -g
CLIENT_COMPILE_GLOBAL = `pkg-config --cflags gtk+-3.0 pkg-config --libs gtk+-3.0`

OBJDIR = obj
SRCDIR = src
UCHAT = uclient

SRC = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS = $(OBJDIR)/*.o

LIBMX_DIR = libraries/libmx
LIBMX_NAME = $(LIBMX_DIR)/libmx.a

SSL_DIR = libraries/openssl
SSL_FILES:= local_lib/lib
SSL_INC_FILES:= $(SSL_DIR)

LIB_FLAGS = -lmx -lssl -lcrypto \
			-L$(LIBMX_DIR) $(LIBMX_NAME) \
			-L$(SSL_FILES) -I$(SSL_INC_FILES)

all: uninstall $(UCHAT)

$(UCHAT):
	mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -c $(SRC) $(CLIENT_COMPILE_GLOBAL)
	mv *.o $(OBJDIR)
	$(CC) $(CFLAGS) -o $(UCHAT) $(OBJS) $(LIB_FLAGS) $(CLIENT_COMPILE_GLOBAL)

clean:
	rm -f $(OBJS)
	rm -df $(OBJDIR)

uninstall:
	make clean
	rm -f $(UCHAT)

reinstall:
	make uninstall
	make all


