CC = clang
# CFLAGS = -std=gnu11 -pthread -g 
# -Wall -Wextra -Werror -Wpedantic
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -pthread -g

OBJDIR = obj
SRCDIR = src
UCHAT = userver

SRC = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS = $(OBJDIR)/*.o

LIBMX_DIR = libraries/libmx
LIBMX_NAME = $(LIBMX_DIR)/libmx.a

SSL_DIR = libraries/openssl
SSL_FILES:= local_lib/lib
SSL_INC_FILES:= $(SSL_DIR)

LIB_FLAGS = -lmx -lsqlite3 -lssl -lcrypto \
			-L$(LIBMX_DIR) $(LIBMX_NAME) \
			-L$(SSL_FILES) -I$(SSL_INC_FILES)

all: uninstall $(UCHAT)

$(UCHAT):
	mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -c $(SRC)
	mv *.o $(OBJDIR)
	$(CC) $(CFLAGS) -o $(UCHAT) $(OBJS) $(LIB_FLAGS)

clean:
	rm -f $(OBJS)
	rm -df $(OBJDIR)

uninstall:
	make clean
	rm -f $(UCHAT)

reinstall:
	make uninstall
	make all


