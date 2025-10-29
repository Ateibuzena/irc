# **************************************************************************** #
#                                   SETTINGS                                   #
# **************************************************************************** #

NAME        := ircserv
CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 -Iincludes
RM          := rm -rf

# Directorios
SRC_DIR     := srcs
OBJ_DIR     := objs

# Buscar todos los .cpp dentro de srcs recursivamente
SRC_FILES   := $(shell find $(SRC_DIR) -type f -name "*.cpp")

# Crear lista de objetos correspondiente
OBJ_FILES   := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@echo "\n🔧 Linking $(NAME)..."
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(NAME)
	@echo "✅ Build complete: $(NAME)"

# Regla para compilar cada archivo fuente
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🧩 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "🧹 Cleaning object files..."
	$(RM) $(OBJ_DIR)

fclean: clean
	@echo "💣 Removing executable..."
	$(RM) $(NAME)

re: fclean all

# Evitar conflictos con archivos llamados igual que reglas
.PHONY: all clean fclean re
