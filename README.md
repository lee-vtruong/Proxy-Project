# Proxy Project - Cross-Platform Network Proxy Server

This project is a cross-platform proxy server application designed to handle network traffic. It includes features such as domain and IP blocking and provides a graphical user interface for ease of management, built with Raylib.

## Table of Contents

- [Proxy Project - Cross-Platform Network Proxy Server](#proxy-project---cross-platform-network-proxy-server)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Features](#features)
  - [Project Structure](#project-structure)
  - [Prerequisites](#prerequisites)
  - [Getting Started](#getting-started)
  - [Configuration](#configuration)
  - [Contributing](#contributing)
  - [Acknowledgments](#acknowledgments)
  - [Team Members](#team-members)

## Overview

This project implements a basic cross-platform network proxy server. It can filter traffic based on configurable lists of blocked domains and IP addresses. A graphical user interface (GUI) is provided to interact with the proxy settings and view status (though basic in this example). The core functionality includes handling incoming connections, parsing HTTP requests, and forwarding/blocking traffic accordingly.

## Features

The project includes the following key features:

-   **Cross-platform Support:** Designed to compile and run on Linux, macOS, and Windows.
-   **Domain and IP Blocking:** Allows users to define lists of domains (`blocked_domains.txt`) and IP addresses (`blocked_ips.txt`) that the proxy should block.
-   **HTTP Parsing:** Includes logic to parse basic HTTP request headers to identify the target host and other relevant information for filtering.
-   **Graphical Interface:** Provides a simple GUI built using the Raylib library for potentially managing proxy settings and viewing status (implementation details depend on the actual GUI code).
-   **Customizable Blocking Lists:** Blocking rules are managed via plain text files that can be easily edited.

## Project Structure

The project is organized into the following main files and directories:
Use code with caution.
Markdown
Proxy-Project/
├── README.md
├── makefile # Script to build the project
├── proxy # Compiled executable
├── asset/ # Assets like fonts and configuration files
│ ├── Consolas.ttf # Font file for the GUI
│ ├── blocked_domains.txt # List of domains to block
│ ├── blocked_ips.txt # List of IP addresses to block
│ ├── instruction.txt # Potential instructions or usage info
│ └── project-creator.txt # Information about the project creator(s)
├── include/ # Header files
│ ├── gui.h
│ ├── common_lib.h
│ ├── cross_platform.h
│ ├── domain_process.h
│ ├── http_parser.h
│ ├── netinc.h # Networking includes (potentially platform-dependent)
│ ├── proxy.h # Main proxy logic header
│ ├── raylib.h # Raylib graphics library header
│ ├── raymath.h # Raylib math utilities header
│ └── rlgl.h # Raylib graphics layer header
├── lib/ # Precompiled Raylib libraries for different platforms
│ ├── Linux/
│ │ └── libraylib.a
│ ├── MacOS/
│ │ └── libraylib.dylib
│ └── Window/
│ └── libraylib.a
└── src/ # Source code files
├── gui.cpp # GUI implementation
├── domain_process.cpp # Logic for processing domain/IP blocking
├── http_parser.cpp # HTTP parsing logic
├── main.cpp # Entry point of the application
├── netimpl.cpp # Network implementation details (sockets etc.)
└── proxy.cpp # Core proxy handling logic
## Prerequisites

Before building and running the project, ensure you have the following installed:

-   **Development Tools**:
    -   **Linux/macOS**: GCC or Clang compiler suite.
    -   **Windows**: MinGW (Minimalist GNU for Windows) or a similar GCC distribution.
-   **Dependencies**:
    -   **Raylib**: The necessary static/dynamic libraries are included in the `lib/` directory for supported platforms. No separate installation is typically required if the `makefile` is used correctly with the included libraries.

## Getting Started

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/NgTrongDoanh/Proxy-Project.git
    ```
2.  **Navigate to the project directory:**
    ```bash
    cd Proxy-Project
    ```
3.  **Build the application:**
    Run the provided `makefile`. This will compile the source files and link them with the Raylib library to create the executable.
    ```bash
    make
    ```
    *Note: The `makefile` is configured for specific environments/compilers. You might need to adjust it based on your system setup.*
4.  **Run the Proxy Server:**
    Execute the compiled program from the project root directory.
    ```bash
    ./proxy
    ```

## Configuration

The blocking behavior of the proxy is controlled by files located in the `asset/` directory:

-   **`asset/blocked_domains.txt`**: Add domains that you want the proxy to block, one domain per line.
    ```
    example.com
    malicious-site.net
    ```
-   **`asset/blocked_ips.txt`**: Add specific IP addresses that you want the proxy to block, one IP per line.
    ```
    192.168.1.100
    10.0.0.5
    ```
Modify these files using a standard text editor. The proxy should load these lists upon startup or when triggered (depending on the GUI/implementation).

## Contributing

If you'd like to contribute to this project, please follow these steps:

1.  Fork the repository.
2.  Create a new branch for your feature or bug fix (`git checkout -b feature/your-feature-name`).
3.  Make your changes and commit them with clear messages (`git commit -m 'feat: Add feature X'` or `fix: Resolve issue Y`).
4.  Push your changes to your fork (`git push origin feature/your-feature-name`).
5.  Submit a pull request to the original repository, describing your changes.

## Acknowledgments

-   Developed using the Raylib library for the graphical user interface.
-   Special thanks to the developers of Raylib and the open-source community for providing valuable tools and resources.

## Team Members

- Nguyễn Trọng Doanh
- Lê Văn Trường
- Nguyễn Lê Khánh

---
