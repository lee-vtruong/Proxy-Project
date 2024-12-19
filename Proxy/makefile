CC = g++
CFLAGS = -Wall -O2 -fPIC
INCLUDES = -Iinclude
TARGET = test

ifeq ($(OS),Windows_NT)
    LDFLAGS = -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 
    RM = del
    EXE = .exe
	SRC = src\GUI.cpp src\http_parser.cpp src\domain_process.cpp
	# SRC = src\domain_process.cpp src\http_parser.cpp src\proxy.cpp
else
    LDFLAGS = -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    RM = rm -f
    EXE =
	SRC = src/GUI.cpp src/http_parser.cpp src/domain_process.cpp
	# SRC = src/main.cpp
	# SRC = src/domain_process.cpp src/http_parser.cpp src/proxy.cpp
endif

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(RM) $(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del src\*.o
	del *.exe
else
	$(RM) $(OBJ) $(TARGET)$(EXE)
endif

run: $(TARGET)$(EXE)
	$(TARGET)$(EXE)
