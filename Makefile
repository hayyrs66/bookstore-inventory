# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iheaders/jsoncpp/include -lssl -lcrypto  # Incluye los headers de jsoncpp
LDFLAGS = -Lheaders/jsoncpp/lib -ljsoncpp  # Enlaza con la librería jsoncpp localmente
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
EXEC = program

# Compilación
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: all
	./$(EXEC)