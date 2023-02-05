NAME	=	uchat

CFLG	=	-std=c11 $(addprefix -W, all extra pedantic) -g -I/usr/include/gtk-2.0 -I/usr/include/atk-1.0 

SRC_DIR	= src
INC_DIR	= inc
OBJ_DIR	= obj

INC_FILES = $(wildcard $(INC_DIR)/*.h)
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))

LMX_DIR	= libs/libmx
LMX_A:=	$(LMX_DIR)/libmx.a
LMX_INC:= $(LMX_DIR)/inc

all: clean install 

install: $(LMX_A) $(NAME)

$(NAME): $(OBJ_FILES)
	@clang $(CFLG) `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0` $(OBJ_FILES) -L$(LMX_DIR) -lmx -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	@clang $(CFLG) `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0` -c $< -o $@ -I$(INC_DIR) -I$(LMX_INC)

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
