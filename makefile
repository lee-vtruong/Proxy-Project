CC = g++
CFLAGS = -Wall -O2 -fPIC
INCLUDES = -Iinclude
TARGET = proxy

ifeq ($(OS),Windows_NT)
    LDFLAGS = -Llib\Window -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32
    RM = del
    EXE = .exe
    SRC = src\main.cpp src\http_parser.cpp src\domain_process.cpp
else 
    RM = rm -f
    EXE =
    SRC = src/main.cpp src/http_parser.cpp src/domain_process.cpp
    ifeq ($(OS), Darwin)
        LDFLAGS = -Llib/MacOS -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework coreVideo
    else
        LDFLAGS = -Llib/Linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	# $(RM) $(OBJ)
	$(RM) src/main.o

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
