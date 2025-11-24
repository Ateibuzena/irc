# **************************************************************************** #
#                                   SETTINGS                                   #
# **************************************************************************** #

NAME        := ircserv
COMPILER	:= c++
FLAGS	    := -Wall -Wextra -Werror -std=c++98 -g -Iincludes 

# Directories
SRC_DIR     := srcs
OBJ_DIR     := objs
DEP_DIR     := dependencies

CHANNEL_PATH	:= logic/channel
CLIENT_PATH		:= logic/client
SLOGIC_PATH		:= logic/serverlogic
PARSER_PATH		:= parser
SERVER_PATH		:= server
UTILS_PATH		:= utils

# All .cpp files
FILES   := 	srcs/logic/channel/Channel.cpp \
				srcs/logic/channel/ChannelMethods.cpp \
				srcs/logic/client/Client.cpp \
				srcs/logic/client/ClientMethods.cpp \
				srcs/logic/serverlogic/AuthCommands.cpp \
				srcs/logic/serverlogic/ChannelCommands.cpp \
				srcs/logic/serverlogic/MessageCommands.cpp \
				srcs/logic/serverlogic/ServerLogic.cpp \
				srcs/parser/Parser.cpp \
				srcs/server/Server.cpp \
				srcs/utils/Utils.cpp \
				srcs/main.cpp

NO_PREFIX := $(patsubst $(SRC_DIR)/%,%,$(FILES))

# Creating objectss based on cpp files
OBJ = $(NO_PREFIX:%.cpp=$(OBJ_DIR)/%.o)

NO_PREFIX_OBJ := $(patsubst $(OBJ_DIR)/%,%,$(OBJ))

# Colors for makefile stetic
BLACK          	=   \033[0;30m
FBLACK          =   \033[1;30m
RED             =   \033[0;31m
FRED            =   \033[1;31m
GREEN           =   \033[0;32m
FGREEN          =   \033[1;32m
BROWNO    		=   \033[0;33m
FBROWNO		   	=   \033[1;33m
FYELLOW         =   \033[1;33m
YELLOW          =   \033[0;33m
BLUE            =   \033[0;34m
FBLUE           =   \033[1;34m
PURPLE          =   \033[0;35m
FPURPLE         =   \033[1;35m
CYAN            =   \033[0;36m
FCYAN           =   \033[1;36m
FWHITE          =   \033[1;37m
WHITE           =   \033[0;37m
RESET           =   \033[0m

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJ_DIR) $(FILES) $(OBJ)
	@echo "$(CYAN) Compiling $(OBJ) into $(NAME)...$(RESET)"
	$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "$(CYAN) 👍 Done compiling 👍$(RESET)"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/$(CHANNEL_PATH) $(OBJ_DIR)/$(CLIENT_PATH) $(OBJ_DIR)/$(SLOGIC_PATH) $(OBJ_DIR)/$(PARSER_PATH) $(OBJ_DIR)/$(SERVER_PATH) $(OBJ_DIR)/$(UTILS_PATH)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(CYAN) Compiling $< into $@...$(RESET)"
	$(COMPILER) $(FLAGS) -MMD -o $@ -c $< 

clean:
	@echo " $(YELLOW)Cleaning $(OBJ) and dependencies ...$(RESET)"
	rm -rf $(OBJ_DIR)
	@echo "$(YELLOW) 🧹 Done cleaning 🧹$(RESET)"

fclean: clean
	@echo " $(YELLOW)Cleaning $(NAME)...$(RESET)"
	rm -f $(NAME)
	@echo "$(YELLOW) 🧹 Done cleaning 🧹$(RESET)"


re: fclean all

# Phony declarations for no-mixing of makefile rules (clean fclean re and all are reserved)
.PHONY: all clean fclean re

# Flag for creating .d files so the includes are also taken into account when compiling project
-include $(OBJ:$(%.o=%.d))
