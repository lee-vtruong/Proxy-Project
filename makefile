CC = g++
CFLAGS = -std=c++17 -Wall -O2 -fPIC
INCLUDES = -Iinclude
TARGET = proxy

ifeq ($(OS),Windows_NT)
    LDFLAGS = -Llib\Window -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32
    RM = del
    EXE = .exe
    SRC = src\netimpl.cpp src\http_parser.cpp src\domain_process.cpp src\gui.cpp src\proxy.cpp src\main.cpp 
else 
    RM = rm -f
    EXE =
    SRC = src/netimpl.cpp src/http_parser.cpp src/domain_process.cpp src/gui.cpp src/proxy.cpp src/main.cpp 
    ifeq ($(shell uname), Linux)
        LDFLAGS = -Llib/Linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    else
        LDFLAGS = -Llib/MacOS -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework coreVideo
    endif
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
