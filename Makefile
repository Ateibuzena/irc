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
LOGIC_PATH		:= logic

PATHS	:= $(OBJ_DIR) \
		   $(OBJ_DIR)/$(CHANNEL_PATH) \
		   $(OBJ_DIR)/$(CLIENT_PATH) \
		   $(OBJ_DIR)/$(SLOGIC_PATH) \
		   $(OBJ_DIR)/$(PARSER_PATH) \
		   $(OBJ_DIR)/$(SERVER_PATH) \
		   $(OBJ_DIR)/$(LOGIC_PATH) \
		   $(OBJ_DIR)/$(UTILS_PATH)

# All .cpp files
FILES   :=	srcs/logic/channel/Channel.cpp \
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
OBJ = $(NO_PREFIX:%.cpp=$(OBJ_DIR)/%.o)

# 🎨 Colors estilo Ana
RED      = \033[0;31m
GREEN    = \033[0;32m
YELLOW   = \033[0;33m
MAGENTA  = \033[0;35m
PINK     = \033[38;2;255;105;180m
CYAN     = \033[0;36m
WHITE    = \033[0;37m
RESET    = \033[0m

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(PATHS) $(OBJ)
	@echo "$(CYAN)🔧 Linking objects to build$(MAGENTA) $(NAME)$(RESET)"
	@$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "$(YELLOW)✅ $(NAME) successfully created.$(RESET)"

$(PATHS):
	@mkdir -p $(PATHS)
	@echo "$(GREEN)📁 Creating object directories...$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN)🔹 Compiling$(MAGENTA) $<$(CYAN) ->$(PINK) $@$(RESET)"
	@$(COMPILER) $(FLAGS) -MMD -c $< -o $@

clean:
	@echo "$(GREEN)🗑️  Removing object directories...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)✅ Object directories removed.$(RESET)"

fclean: clean
	@if [ -f "$(NAME)" ]; then \
		echo "$(GREEN)🚮 Removing $(NAME)...$(RESET)"; \
		rm -f $(NAME); \
		echo "$(YELLOW)✅ $(NAME) removed.$(RESET)"; \
	fi

re: fclean all

.PHONY: all clean fclean re

-include $(OBJ:.o=.d)
