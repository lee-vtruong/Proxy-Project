#define RAYGUI_IMPLEMENTATION

#include "../include/raygui.h"
#include "../include/raylib.h"
#include "../include/http_parser.h"

class MessageRecord {
    private:
        std::vector<ConnectionInfo> messages;
        Rectangle recordRect = {};
        int scrollIndex = 0;
        int selectedRow = -1;
        bool showDetails = false;
        int VISIBLE_ROWS = 7;
        int ROW_HEIGHT = 20;
        Font FONT;
        Color TEXT_COLOR;
        Color BG_COLOR;
        Rectangle scrollBar;

    public:
        MessageRecord(Rectangle rect) {
            recordRect = rect;
            scrollBar = {760, 50, 20, float(VISIBLE_ROWS * ROW_HEIGHT)};
            scrollIndex = GuiScrollBar(scrollBar, scrollIndex, 0, 1);

        }
        MessageRecord() {
            recordRect = {10, 50, 100, 20};
        }

        void handle() {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                for (int i = 0; i < VISIBLE_ROWS; i++) {
                    Rectangle rowRect = {10, 50 + i * ROW_HEIGHT, 740, ROW_HEIGHT};
                    if (CheckCollisionPointRec(mousePos, rowRect)) {
                        selectedRow = scrollIndex + i;  // Lấy hàng tương ứng
                        showDetails = true;
                        break;
                    }
                }
            }
            scrollIndex = GuiScrollBar(scrollBar, scrollIndex, 0, messages.size() - VISIBLE_ROWS);
        }

        void draw() {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("HTTP Messages Table", 10, 10, 20, BLACK);
            GuiLabel((Rectangle){10, 30, 100, 20}, "Method");
            GuiLabel((Rectangle){120, 30, 150, 20}, "IP Source");
            GuiLabel((Rectangle){280, 30, 150, 20}, "IP Dest");
            GuiLabel((Rectangle){440, 30, 250, 20}, "URL");
            GuiLabel((Rectangle){700, 30, 80, 20}, "Resp.");
            for (int i = 0; i < VISIBLE_ROWS; i++) {
                int messageIndex = scrollIndex + i;  // Lấy dữ liệu của hàng
                if (messageIndex >= messages.size()) break;
                Rectangle rowRect = {10, 50 + i * ROW_HEIGHT, 740, ROW_HEIGHT};
                if (messageIndex == selectedRow) {
                    DrawRectangleRec(rowRect, LIGHTGRAY);
                }

                GuiLabel((Rectangle){10, 50 + i * ROW_HEIGHT, 100, 20}, messages[messageIndex].transactions[0].request.method.c_str()); 
                GuiLabel((Rectangle){120, 50 + i * ROW_HEIGHT, 150, 20}, messages[messageIndex].client.ip);
                GuiLabel((Rectangle){280, 50 + i * ROW_HEIGHT, 150, 20}, messages[messageIndex].server.ip);
                GuiLabel((Rectangle){440, 50 + i * ROW_HEIGHT, 250, 20}, messages[messageIndex].transactions[0].request.uri.c_str());
                GuiLabel((Rectangle){700, 50 + i * ROW_HEIGHT, 80, 20}, std::to_string(messages[messageIndex].transactions[0].response.statusCode).c_str());
            }

            DrawRectangleRec(recordRect, Fade(GRAY, 0.6f));

            if (showDetails && selectedRow != -1) {
                ConnectionInfo connection = messages[selectedRow];
                DrawRectangle(10, 50, 740, 20, Fade(GRAY, 0.6f));
                DrawRectangleLines(10, 50, 740, 20, Fade(DARKGRAY, 0.6f));

                DrawText("Connection Details", 20, 60, 20, BLACK);
                DrawText(TextFormat("Method: %s", connection.transactions[0].request.method.c_str()), 20, 100, 18, BLACK);
                DrawText(TextFormat("URL: %s", connection.transactions[0].request.uri.c_str()), 20, 120, 18, BLACK);
                DrawText(TextFormat("IP Source: %s", connection.client.ip), 20, 140, 18, BLACK);
                DrawText(TextFormat("IP Dest: %s", connection.server.ip), 20, 160, 18, BLACK);
                DrawText(TextFormat("Response Code: %d", connection.transactions[0].response.statusCode), 20, 180, 18, BLACK);
                if (connection.transactions[0].request.isEncrypted) {
                    DrawText("Encrypted: Yes", 20, 200, 18, BLACK);
                } else {
                    DrawText("Encrypted: No", 20, 200, 18, BLACK);
                }
                if (GuiButton((Rectangle){20, 220, 100, 30}, "Close")) {
                    showDetails = false;
                    selectedRow = -1;
                }
            }

            EndDrawing();
        }
};

class Application {
    private:
        float HEIGHT = 900.f, WIDTH = 1600.f;
        MessageRecord messageRecord;

    public:
        void InitApp() {
            InitWindow(WIDTH, HEIGHT, "Proxy Server");
            SetTargetFPS(60);
            messageRecord = MessageRecord({10, 50, 740, 400});
            

        }
};