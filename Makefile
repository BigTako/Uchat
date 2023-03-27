# OBJDIR = obj
# SRCDIR = src
# SERVER_DIR = server
# CLIENT_DIR = client
# SERVER = userver
# CLIENT = uclient

# SRC = $(SRCDIR)/*.c
# OBJS = $(OBJDIR)/*.o

# all: $(SERVER) $(CLIENT)

# $(SERVER):
# 	make -sC $(SERVER_DIR)

# $(CLIENT):
# 	make -sC $(CLIENT_DIR)

# clean:
# 	rm -f $(OBJS)
# 	rm -df $(OBJDIR) 

# uninstall:
# 	make -sC $(SERVER_DIR) $@
# 	make -sC $(CLIENT_DIR) $@
# 	make clean
# 	rm -f $(UCHAT)

# reinstall:
# 	make uninstall
# 	make all

.PHONY: all install uninstall reinstall

all: install

install:
	echo "[INFO] installing client\n"
	@$(MAKE) reinstall -C client/
	echo "[INFO] installing server\n"
	@$(MAKE) reinstall -C server/
	@cp client/uchat ./
	@cp server/uchat_server ./

uninstall:
	@$(MAKE) -C server/ uninstall
	@$(MAKE) -C client/ uninstall
	@rm uchat
	@rm uchat_server

reinstall: uninstall install
