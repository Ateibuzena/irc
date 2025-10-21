# ========= Config =========
NAME        := irc_server
CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 -Iinc -MMD -MP

SRC_DIR     := src
INC_DIR     := inc
SRC_OBJ_DIR := $(SRC_DIR)/obj
INC_OBJ_DIR := $(INC_DIR)/obj

# ========= Sources (recursive) =========
# Todos los .cpp en src (recursivo)
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
# Objetos en src/obj replicando estructura
SRC_OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(SRC_OBJ_DIR)/%.o,$(SRC_FILES))

# (Opcional) headers en inc (recursivo)
INC_FILES := $(shell find $(INC_DIR) -type f -name '*.hpp')
INC_OBJS  := $(patsubst $(INC_DIR)/%.hpp,$(INC_OBJ_DIR)/%.o,$(INC_FILES))

DEPS := $(SRC_OBJS:.o=.d)

# ========= Rules =========
all: $(NAME)

$(NAME): $(SRC_OBJS)
	@echo "🔧 Linking $(NAME)"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✅ Build complete"

# compila .cpp -> .o (crea subcarpetas en src/obj si hacen falta)
$(SRC_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🧱 $(<)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# (Opcional) “procesa” headers -> obj (solo para uniformidad de carpetas)
$(INC_OBJ_DIR)/%.o: $(INC_DIR)/%.hpp
	@mkdir -p $(dir $@)
	@echo "📦 header $<"
	@touch $@

clean:
	@echo "🧹 Cleaning objects"
	@rm -rf $(SRC_OBJ_DIR) $(INC_OBJ_DIR)

fclean: clean
	@echo "🧽 Removing binary"
	@rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
