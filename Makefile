# Nombre del ejecutable
TARGET = bookstore

# Compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -std=c++17 -Wall -Wextra -g -fsanitize=address  # Añadido -g para depuración y -fsanitize=address para detectar errores de memoria

# Archivos fuente y de cabecera
SRCS = Book.cpp BTree.cpp BTreeNode.cpp Inventary.cpp Testing.cpp books.cpp
HEADERS = Book.h BTree.h BTreeNode.h Inventary.h Testing.h

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Regla por defecto
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -ljsoncpp

# Ejecutar y guardar salida en debug.txt (redirigir stdout y stderr)
run: $(TARGET)
	./$(TARGET) > debug.txt 2>&1

# Ejecutar con Valgrind y guardar la salida en valgrind_output.txt
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) > valgrind_output.txt 2>&1

# Regla para compilar los archivos objeto
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos intermedios
clean:
	rm -f $(OBJS) $(TARGET)

# Limpiar todo
distclean: clean
	rm -f $(TARGET)
