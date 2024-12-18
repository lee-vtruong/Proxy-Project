
#include "../include/GUI.h"
#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include <sstream>
#include <string>
#include <vector>
#include "../include/proxy.h"

// // #include "GUI.h" // Include lớp Popup và TextBox
// int main() {
//     // Khởi tạo cửa sổ raylib
//     InitWindow(800, 600, "Table Example");
//     SetTargetFPS(60);

//     // Tạo dữ liệu ConnectionInfo
//     std::vector<ConnectionInfo> connections;
//     for (int i = 0; i < 50; i++) {
//         ConnectionInfo connection;
//         snprintf(connection.client.ip, INET_ADDRSTRLEN, "192.168.1.%d", i);
//         connection.client.port = 1000 + i;
//         snprintf(connection.server.ip, INET_ADDRSTRLEN, "10.0.0.%d", i);
//         connection.server.port = 80;
//         HttpRequest request = { "", "GET", "/index.html", "HTTP/1.1", {{"Host", "example.com"}}, "", false };
//         HttpResponse response = { "", "HTTP/1.1", 200, "OK", {{"Content-Type", "text/html"}}, "<html>...</html>", false };
//         connection.addTransaction(request, response);
//         connections.push_back(connection);
//     }
//     Font Roboto = LoadFont("asset/Roboto.ttf");
//     // Tạo bảng
//     Table table(50, 50, 700, 500, connections, Roboto);

//     // Vòng lặp chính
//     while (!WindowShouldClose()) {
//         Vector2 mousePosition = GetMousePosition();

//         // Cập nhật bảng
//         table.Update(mousePosition);

//         // Vẽ giao diện
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         table.Draw();

//         EndDrawing();
//     }

//     // Đóng cửa sổ
//     CloseWindow();

//     return 0;
// }



// int main() {
//     // Khởi tạo cửa sổ
//     const int screenWidth = 800;
//     const int screenHeight = 600;
//     InitWindow(screenWidth, screenHeight, "Demo sử dụng Button, ToggleButton, TextBox và Popup");
//     SetTargetFPS(60);

//     // Font mặc định
//     Font defaultFont = LoadFont("asset/Roboto.ttf");

//     // Tạo các đối tượng
//     Button myButton(50, 50, 150, 50, "Click Me", 20, 10.0f, defaultFont);
//     ToggleButton toggleButton(50, 150, 150, 50, "ON", "OFF", 20, 10.0f, defaultFont, GREEN, LIGHTGRAY, DARKGRAY, WHITE);
//     TextBox textBox(300, 50, 400, 300, "This is a demo TextBox.\nYou can scroll through the content.\nScroll down for more lines.\nLine 4\nLine 5\nLine 6\nLine 7\nLine 8", defaultFont, 18, 10.0f);
//     Popup popup(200, 100, 400, 300, "Popup Title", "This is the content inside the popup.\nIt supports scrolling and closing.", defaultFont);

//     // Trạng thái hiển thị Popup
//     bool showPopup = true;

//     while (!WindowShouldClose()) {
//         // Cập nhật các đối tượng
//         myButton.Update();
//         toggleButton.Update();
//         textBox.Update();

//         if (showPopup) {
//             showPopup = popup.Update();
//         }

//         // Xử lý sự kiện click của Button
//         if (myButton.IsClicked()) {
//             std::cout << "Button clicked!" << std::endl;
//             showPopup = true; // Hiển thị popup khi nhấn nút
//         }

//         // Vẽ giao diện
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         DrawText("Demo UI với raylib", 10, 10, 20, DARKGRAY);

//         myButton.Draw();
//         toggleButton.Draw();
//         textBox.Draw();

//         if (showPopup) {
//             popup.Draw();
//         }

//         // Hiển thị trạng thái của ToggleButton
//         DrawText(toggleButton.GetState() ? "Toggle State: ON" : "Toggle State: OFF", 50, 220, 20, BLACK);

//         EndDrawing();
//     }

//     // Giải phóng tài nguyên
//     CloseWindow();
//     return 0;
// }



// int main() {
//     // Khởi tạo cửa sổ
//     InitWindow(800, 600, "InputField with Button Example");

//     Font defaultFont = LoadFont("asset/Roboto.ttf");
//     InputFieldWithButton inputWithButton(200, 200, 300, 50, "Submit", 520, 200, 100, 50, defaultFont);

//     SetTargetFPS(60);

//     while (!WindowShouldClose()) {
//         // Cập nhật
//         inputWithButton.Update();

//         // Vẽ
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         inputWithButton.Draw();

//         // Kiểm tra button được nhấn
//         if (inputWithButton.IsButtonClicked()) {
//             DrawText("Button Pressed!", 200, 300, 20, RED);
//             DrawText(inputWithButton.GetInputText().c_str(), 200, 350, 20, BLUE);
//         }

//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }


int main() {
    InitWindow(1600, 900, "Name List Manager");

    // Load font tùy chỉnh
    Font customFont = LoadFont("asset/Roboto.ttf");

    // Đọc dữ liệu từ file
    std::string FilePath = "asset/blocked_ip.txt";
    std::unordered_set<std::string> nameSet;
    std::ifstream File(FilePath);
    std::string line;
    while (std::getline(File, line)) {
        nameSet.insert(line);
    }
    File.close();

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
    Proxy hehe(8080);
    // Tạo NameList
    Table table(50, 200, 800, 500, connections, customFont);
    NameList nameList(FilePath, 950, 350, 600, 200, hehe.BLACK_LIST.domains, customFont, "Blocked Domain List");
    NameList nameList1(FilePath, 950, 125, 600, 200, hehe.BLACK_LIST.ips, customFont, "Blocked IP List");
    InputFieldWithButton inputFieldWithButton(150, 125, 100, 50, "Change Port", 300, 125, 150, 50, customFont);
    inputFieldWithButton.SetText(std::to_string(hehe.getPort()));
    ToggleButton toggleButton(550, 125, 200, 50, "Stop Proxy", "Start Proxy", 20, 10.0f, customFont, GREEN, LIGHTGRAY, DARKGRAY, WHITE);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        // // Update
        nameList.Update();
        nameList1.Update();
        inputFieldWithButton.Update();
        
        table.Update(GetMousePosition());
        int flag = toggleButton.Update();
        
        if (flag == 1) {
            hehe.start();
            std::thread(&Proxy::acceptConnections, &hehe).detach();
        } else if (flag == -1) {
            hehe.stop();
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextEx(customFont, "Right-click to delete a name", {220, 70}, 20, 1, GRAY);
        nameList.Draw();
        nameList1.Draw();
        inputFieldWithButton.Draw();
        toggleButton.Draw();
        table.Draw();

        EndDrawing();
    }

    // Dọn dẹp tài nguyên
    UnloadFont(customFont);
    CloseWindow();
    return 0;
}