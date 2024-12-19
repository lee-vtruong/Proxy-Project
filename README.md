# Proxy Project

## Overview
This project is a cross-platform proxy server application designed to handle network traffic. It includes features such as domain and IP blocking and provides a graphical user interface for ease of management.

## Features
- **Cross-platform Support**: Runs on Linux, macOS, and Windows.
- **Domain and IP Blocking**: Configurable lists for blocking unwanted traffic.
- **HTTP Parsing**: Efficient parsing of HTTP requests and responses.
- **Graphical Interface**: User-friendly GUI built with Raylib for managing settings.

## File Structure
```
Proxy-Project/
├── README.md
├── makefile
├── proxy
├── asset/
│   ├── Consolas.ttf
│   ├── blocked_domains.txt
│   ├── blocked_ips.txt
│   ├── instruction.txt
│   └── project-creator.txt
├── include/
│   ├── gui.h
│   ├── common_lib.h
│   ├── cross_platform.h
│   ├── domain_process.h
│   ├── http_parser.h
│   ├── netinc.h
│   ├── proxy.h
│   ├── raylib.h
│   ├── raymath.h
│   └── rlgl.h
├── lib/
│   ├── Linux/
│   │   └── libraylib.a
│   ├── MacOS/
│   │   └── libraylib.dylib
│   └── Window/
│       └── libraylib.a
└── src/
    ├── gui.cpp
    ├── domain_process.cpp
    ├── http_parser.cpp
    ├── main.cpp
    ├── netimpl.cpp
    └── proxy.cpp
```

## Prerequisites

- **Development Tools**:
  - GCC or Clang for Linux, MacOS
  - MinGW for Windows
- **Dependencies**:
  - Raylib (included in the `lib/` folder).

## Build and Run

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd Proxy-Project
   ```

2. **Build the Application**:
   Run the provided `makefile`:
   ```bash
   make
   ```

3. **Run the Proxy Server**:
   ```bash
   ./proxy
   ```

## Configuration

- **Blocking Settings**:
  Edit the files in the `asset/` directory:
  - `blocked_domains.txt`: Add domains to block, one per line.
  - `blocked_ips.txt`: Add IPs to block, one per line.

## Contribution

Feel free to submit pull requests or report issues. Contributions are always welcome!

## Acknowledgments

- Developed using Raylib for graphical interface.
- Special thanks to the contributors and the open-source community.

---

# Dự án Proxy

## Tổng quan
Dự án này là một ứng dụng máy chủ proxy đa nền tảng được thiết kế để xử lý lưu lượng mạng. Nó bao gồm các tính năng như chặn tên miền và IP, đồng thời cung cấp giao diện người dùng đồ họa để quản lý dễ dàng.

## Tính năng
- **Hỗ trợ đa nền tảng**: Chạy trên Linux, macOS và Windows.
- **Chặn tên miền và IP**: Danh sách cấu hình để chặn lưu lượng không mong muốn.
- **Phân tích HTTP**: Phân tích hiệu quả các yêu cầu và phản hồi HTTP.
- **Giao diện đồ họa**: Giao diện người dùng thân thiện được xây dựng bằng Raylib để quản lý các cài đặt.

## Cấu trúc tệp
```
Proxy-Project/
├── README.md
├── makefile
├── proxy
├── asset/
│   ├── Consolas.ttf
│   ├── blocked_domains.txt
│   ├── blocked_ips.txt
│   ├── instruction.txt
│   └── project-creator.txt
├── include/
│   ├── gui.h
│   ├── common_lib.h
│   ├── cross_platform.h
│   ├── domain_process.h
│   ├── http_parser.h
│   ├── netinc.h
│   ├── proxy.h
│   ├── raylib.h
│   ├── raymath.h
│   └── rlgl.h
├── lib/
│   ├── Linux/
│   │   └── libraylib.a
│   ├── MacOS/
│   │   └── libraylib.dylib
│   └── Window/
│       └── libraylib.a
└── src/
    ├── gui.cpp
    ├── domain_process.cpp
    ├── http_parser.cpp
    ├── main.cpp
    ├── netimpl.cpp
    └── proxy.cpp
```

## Yêu cầu

- **Công cụ phát triển**:
  - GCC hoặc Clang cho Linux, MacOS
  - MinGW cho Windows
- **Phụ thuộc**:
  - Raylib (đã bao gồm trong thư mục `lib/`).

## Xây dựng và chạy

1. **Sao chép kho lưu trữ**:
   ```bash
   git clone <repository-url>
   cd Proxy-Project
   ```

2. **Xây dựng ứng dụng**:
   Chạy `makefile` đã cung cấp:
   ```bash
   make
   ```

3. **Chạy máy chủ proxy**:
   ```bash
   ./proxy
   ```

## Cấu hình

- **Cài đặt chặn**:
  Chỉnh sửa các tệp trong thư mục `asset/`:
  - `blocked_domains.txt`: Thêm các tên miền để chặn, mỗi tên miền một dòng.
  - `blocked_ips.txt`: Thêm các IP để chặn, mỗi IP một dòng.

## Đóng góp

Hãy gửi pull request hoặc báo cáo lỗi. Luôn hoan nghênh các đóng góp!

## Giấy phép

Dự án này được cấp phép theo [MIT License](LICENSE).

## Ghi nhận

- Được phát triển bằng Raylib cho giao diện đồ họa.
- Đặc biệt cảm ơn các nhà đóng góp và cộng đồng mã nguồn mở.

# Team members

- Nguyễn Trọng Doanh
- Lê Văn Trường
- Nguyễn Lê Khánh