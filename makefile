CXX = g++

LDFLAGS = -lpthread 

# Tệp đích
TARGET = proxy

# Các tệp mã nguồn
SRC_DIR = src
SRC_FILES = $(SRC_DIR)/common_lib.cpp $(SRC_DIR)/proxy.cpp $(SRC_DIR)/domain_process.cpp $(SRC_DIR)/http_parser.cpp

# Luật để biên dịch toàn bộ chương trình
$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_FILES) $(LDFLAGS)

# Luật dọn dẹp tệp sinh ra
clean:
	rm -f $(TARGET)
