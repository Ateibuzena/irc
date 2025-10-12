# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME        := ./testIRC
CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 \
               -I../../includes \
               -I../../includes/logic

SRC_DIR     := ../../srcs/logic
OBJ_DIR     := obj

SRC         := $(SRC_DIR)/client/Client.cpp \
               $(SRC_DIR)/client/ClientUtils.cpp \
               $(SRC_DIR)/channel/Channel.cpp \
               $(SRC_DIR)/channel/ChannelUtils.cpp \
               $(SRC_DIR)/server/ServerLogic.cpp \
               main.cpp

OBJ         := $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.o)))

GREEN       := \033[0;32m
YELLOW      := \033[1;33m
RED         := \033[0;31m
RESET       := \033[0m

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(YELLOW)🔧 Linking $(NAME)...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)✅ Build complete: $(NAME)$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp | $(OBJ_DIR)
	@echo "$(YELLOW)Compiling source:$(RESET) $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "$(YELLOW)Compiling test:$(RESET) $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "$(RED)🧹 Removing object files...$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)💣 Removing executable...$(RESET)"
	@rm -f $(NAME)

re: fclean all

run: all
	@echo "$(YELLOW)🚀 Running test...$(RESET)"
	@./$(NAME)

.PHONY: all clean fclean re run
