NAME	=	uclient

CFLG	=	-std=c11 $(addprefix -W, all extra pedantic) -g -Wno-unused-command-line-argument -Wno-unused-variable \
    -Wno-unused-function -Wno-unused-parameter
CLIENT_COMPILE_GLOBAL = `pkg-config --cflags gtk+-3.0 pkg-config --libs gtk+-3.0`

SRC_DIR	= src
INC_DIR	= inc
OBJ_DIR	= obj

INC_FILES = $(wildcard $(INC_DIR)/*.h)
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))

LMX_DIR	= libs/libmx
LMX_A:=	$(LMX_DIR)/libmx.a
LMX_INC:= $(LMX_DIR)/inc

SSL_DIR = libs/openssl
SSL_FILES:= local_lib/lib
SSL_INC_FILES:= $(SSL_DIR)

LIB_FLAGS = -lmx -lsqlite3 -lssl -lcrypto \
			-L$(SSL_FILES) -I$(SSL_INC_FILES)

all: clean install 

install: $(LMX_A) $(NAME)

$(NAME): $(OBJ_FILES)
	@clang $(CFLG) $(OBJ_FILES) -L$(LMX_DIR) -lmx -o $@ $(CLIENT_COMPILE_GLOBAL) $(LIB_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	@clang $(CFLG) -c $< -o  $@ -I$(INC_DIR) -I$(LMX_INC) $(CLIENT_COMPILE_GLOBAL) $(LIB_FLAGS)

$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir -p $@

$(LMX_A):
	@make -sC $(LMX_DIR)
	
clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(NAME)

uninstall:
	@make -sC $(LMX_DIR) $@
	@rm -rf $(OBJ_DIR)
	@rm -rf $(NAME)

reinstall: uninstall all
