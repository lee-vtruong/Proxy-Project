#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_MESSAGES 20  // Số lượng thông điệp tối đa
#define ROW_HEIGHT 30    // Chiều cao mỗi hàng
#define VISIBLE_ROWS 5   // Số lượng hàng hiển thị trong khung

typedef struct HttpMessage {
    char method[10];
    char ipSource[20];
    char ipDest[20];
    char url[50];
    int responseCode;
} HttpMessage;

int main() {
    InitWindow(800, 600, "Scrollable HTTP Messages Table");
    SetTargetFPS(60);
    Font roboto = LoadFontEx("asset/Consolas.ttf", 64, 0, 0);
    GuiSetFont(roboto);

    // Dữ liệu mẫu
    HttpMessage messages[MAX_MESSAGES] = {
        {"GET", "192.168.1.1", "93.184.216.34", "example.com", 200},
        {"POST", "10.0.0.2", "172.217.3.110", "google.com", 404},
        {"PUT", "192.168.0.5", "151.101.65.69", "github.com", 201},
        {"DELETE", "192.168.1.8", "192.30.255.113", "bitbucket.org", 500},
        {"GET", "172.16.0.3", "216.58.204.78", "youtube.com", 403},
        {"PATCH", "192.168.1.3", "13.227.252.25", "amazon.com", 200},
        {"HEAD", "10.0.0.4", "17.253.144.10", "apple.com", 301},
        {"OPTIONS", "192.168.100.1", "104.26.5.119", "cloudflare.com", 200},
        {"TRACE", "10.0.0.1", "185.199.108.153", "github.io", 504},
        {"CONNECT", "172.16.1.7", "142.250.64.78", "googleapis.com", 302},
        {"GET", "192.168.1.10", "8.8.8.8", "example.net", 200},
        {"POST", "172.16.1.15", "1.1.1.1", "test.com", 404},
        {"PUT", "192.168.1.12", "255.255.255.255", "overflow.com", 500},
        {"DELETE", "192.168.1.25", "192.0.2.1", "dummy.com", 302},
        {"PATCH", "192.168.1.6", "198.51.100.1", "placeholder.com", 403},
        {"GET", "192.168.1.8", "203.0.113.1", "localtest.com", 503},
        {"OPTIONS", "192.168.1.3", "203.0.113.2", "example.local", 200},
        {"HEAD", "192.168.1.5", "127.0.0.1", "loopback.com", 304},
        {"TRACE", "192.168.1.11", "192.168.1.254", "gateway.com", 404},
        {"CONNECT", "192.168.1.7", "192.168.1.100", "localnet.com", 201}
    };

    // Thanh cuộn
    int scrollIndex = 0;           // Chỉ số dòng đầu tiên hiển thị
    int totalRows = MAX_MESSAGES;  // Tổng số hàng trong bảng

    // Chọn hàng
    int selectedRow = -1;
    bool showDetails = false;

    while (!WindowShouldClose()) {
        // Thanh cuộn dọc
        Rectangle scrollBar = {760, 50, 20, VISIBLE_ROWS * ROW_HEIGHT};
        scrollIndex = GuiScrollBar(scrollBar, scrollIndex, 0, totalRows - VISIBLE_ROWS);

        // Xử lý nhấp chuột
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            for (int i = 0; i < VISIBLE_ROWS; i++) {
                Rectangle rowRect = {10, 50 + i * ROW_HEIGHT, 740, ROW_HEIGHT};
                if (CheckCollisionPointRec(mousePos, rowRect)) {
                    selectedRow = scrollIndex + i;  // Lấy hàng tương ứng
                    showDetails = true;            // Hiển thị chi tiết
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Tiêu đề bảng
        DrawText("HTTP Messages Table", 10, 10, 20, BLACK);

        // Header
        GuiLabel((Rectangle){10, 30, 100, 20}, "Method");
        GuiLabel((Rectangle){120, 30, 150, 20}, "IP Source");
        GuiLabel((Rectangle){280, 30, 150, 20}, "IP Dest");
        GuiLabel((Rectangle){440, 30, 250, 20}, "URL");
        GuiLabel((Rectangle){700, 30, 80, 20}, "Resp.");

        // Hiển thị các hàng trong khung
        for (int i = 0; i < VISIBLE_ROWS; i++) {
            int messageIndex = scrollIndex + i;  // Lấy dữ liệu của hàng

            // Kiểm tra nếu hàng nằm ngoài danh sách
            if (messageIndex >= totalRows) break;

            Rectangle rowRect = {10, 50 + i * ROW_HEIGHT, 740, ROW_HEIGHT};

            // Highlight hàng được chọn
            if (messageIndex == selectedRow) {
                DrawRectangleRec(rowRect, LIGHTGRAY);
            }

            // Hiển thị thông tin từng cột
            GuiLabel((Rectangle){10, 50 + i * ROW_HEIGHT, 100, 20}, messages[messageIndex].method);
            GuiLabel((Rectangle){120, 50 + i * ROW_HEIGHT, 150, 20}, messages[messageIndex].ipSource);
            GuiLabel((Rectangle){280, 50 + i * ROW_HEIGHT, 150, 20}, messages[messageIndex].ipDest);
            GuiLabel((Rectangle){440, 50 + i * ROW_HEIGHT, 250, 20}, messages[messageIndex].url);

            char responseCode[10];
            sprintf(responseCode, "%d", messages[messageIndex].responseCode);
            GuiLabel((Rectangle){700, 50 + i * ROW_HEIGHT, 80, 20}, responseCode);

            // Viền của mỗi hàng
            DrawRectangleLinesEx(rowRect, 1, BLACK);
        }

        // Hiển thị chi tiết
        if (showDetails && selectedRow != -1) {
            DrawRectangle(100, 100, 600, 400, Fade(LIGHTGRAY, 0.9f));
            DrawRectangleLines(100, 100, 600, 400, BLACK);

            DrawText("Connection Details", 120, 120, 20, BLACK);
            DrawText(TextFormat("Method: %s", messages[selectedRow].method), 120, 160, 18, BLACK);
            DrawText(TextFormat("IP Source: %s", messages[selectedRow].ipSource), 120, 190, 18, BLACK);
            DrawText(TextFormat("IP Dest: %s", messages[selectedRow].ipDest), 120, 220, 18, BLACK);
            DrawText(TextFormat("URL: %s", messages[selectedRow].url), 120, 250, 18, BLACK);
            DrawText(TextFormat("Response Code: %d", messages[selectedRow].responseCode), 120, 280, 18, BLACK);

            if (GuiButton((Rectangle){600, 440, 80, 30}, "Close")) {
                showDetails = false;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
