# **************************************************************************** #
#                               CONFIGURATION                                   #
# **************************************************************************** #

NAME        := testChannel
CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 \
               -I../../../includes \
               -I../../../includes/logic

SRC_DIR_CLIENT := ../../../srcs/logic/client
SRC_DIR_CHANNEL := ../../../srcs/logic/channel

OBJ_DIR     := obj

SRC         := $(SRC_DIR_CHANNEL)/Channel.cpp \
				$(SRC_DIR_CHANNEL)/ChannelUtils.cpp \
               $(SRC_DIR_CLIENT)/Client.cpp \
               $(SRC_DIR_CLIENT)/ClientUtils.cpp \
               TestChannel.cpp

OBJ         := $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.o)))

# Colores
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

# Compilar objetos de srcs
$(OBJ_DIR)/%.o: $(SRC_DIR_CLIENT)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	@echo "$(YELLOW)Compiling client source:$(RESET) $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_CHANNEL)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	@echo "$(YELLOW)Compiling channel source:$(RESET) $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar test
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
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

# **************************************************************************** #
#                                   PHONY                                      #
# **************************************************************************** #

.PHONY: all clean fclean re run
