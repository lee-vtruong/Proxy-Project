
#include "../include/GUI.h"
#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include <sstream>
#include <string>
#include <vector>
// #include "GUI.h" // Include lớp Popup và TextBox
int main() {
    // Khởi tạo cửa sổ raylib
    InitWindow(800, 600, "Table Example");
    SetTargetFPS(60);

    // Tạo dữ liệu ConnectionInfo
    std::vector<ConnectionInfo> connections;
    for (int i = 0; i < 50; i++) {
        ConnectionInfo connection;
        snprintf(connection.client.ip, INET_ADDRSTRLEN, "192.168.1.%d", i);
        connection.client.port = 1000 + i;
        snprintf(connection.server.ip, INET_ADDRSTRLEN, "10.0.0.%d", i);
        connection.server.port = 80;
        HttpRequest request = { "", "GET", "/index.html", "HTTP/1.1", {{"Host", "example.com"}}, "", false };
        HttpResponse response = { "", "HTTP/1.1", 200, "OK", {{"Content-Type", "text/html"}}, "<html>...</html>", false };
        connection.addTransaction(request, response);
        connections.push_back(connection);
    }
    Font Roboto = LoadFont("asset/Roboto.ttf");
    // Tạo bảng
    Table table(50, 50, 700, 500, connections, Roboto);

    // Vòng lặp chính
    while (!WindowShouldClose()) {
    Vector2 mousePosition = GetMousePosition();

    // Cập nhật bảng
    table.Update(mousePosition);


        // Vẽ giao diện
        BeginDrawing();
        ClearBackground(RAYWHITE);

        table.Draw();

        EndDrawing();
    }

    // Đóng cửa sổ
    CloseWindow();

    return 0;
}
