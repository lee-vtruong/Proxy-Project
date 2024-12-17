CC = g++
CFLAGS = -Wall -O2 -fPIC
LDFLAGS = -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
INCLUDES = -Iinclude         

TARGET = test

# SRC = src/domain_process.cpp src/http_parser.cpp src/proxy.cpp
SRC = src/GUI.cpp src/http_parser.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	rm -f $(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
