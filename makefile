CXX = g++
LDFLAGS = -lpthread 

TARGET = proxy

SRC_DIR = src
SRC_FILES = $(SRC_DIR)/common_lib.cpp $(SRC_DIR)/proxy.cpp $(SRC_DIR)/domain_process.cpp $(SRC_DIR)/http_parser.cpp

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_FILES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
