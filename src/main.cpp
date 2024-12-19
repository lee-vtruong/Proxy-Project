#include "../include/GUI.h"
#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include <sstream>
#include <string>
#include <vector>
#include "../include/proxy.h"

std::string readFile(const char* filename) {
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Không thể mở tệp: " << filename << std::endl;
        return {}; // Trả về lỗi nếu tệp không mở được
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Chuyển toàn bộ nội dung tệp vào `buffer`

    std::string fileContent = buffer.str();

    return fileContent;
}


int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1600, 900, "Proxy Server");

    Font customFont = LoadFont("asset/Consolas.ttf");
    Font titleFont = LoadFontEx("asset/Consolas.ttf", 128, NULL, 0);

    const char* domainFile = "asset/blocked_domains.txt";
    const char* ipFile = "asset/blocked_ips.txt";

    // std::unordered_set<std::string> nameSet;
    // std::ifstream File(FilePath);
    // std::string line;
    // while (std::getline(File, line)) {
        // nameSet.insert(line);
    // }
    // File.close();

    Proxy proxy(8080);
    // socket_t proxy_fd = INVALID_SOCKET;

    Table connectionRecord(50, 200, 800, 500, proxy.connections, customFont);

    NameList blockedDomain(domainFile, 950, 350, 600, 200, proxy.BLACK_LIST.domains, customFont, "Blocked Domain List");
    NameList blockedIp(ipFile, 950, 125, 600, 200, proxy.BLACK_LIST.ips, customFont, "Blocked IP List");

    InputFieldWithButton portButton(150, 135, 100, 30, "Change Port", 300, 130, 150, 40, customFont);
    portButton.SetText(std::to_string(proxy.getPort()));

    ToggleButton startButton(550, 125, 200, 50, "Stop Proxy", "Start Proxy", 20, 10.0f, customFont, PRIMARY_BUTTON_COLOR, PRIMARY_HOVERED_BUTTON_COLOR, PRESS_COLOR, WHITE, WHITE);


    TextBox Usage(950, 600, 600, 275, readFile("asset/instruction.txt"), customFont);
    TextBox dev(50, 725, 800, 150, readFile("asset/project-creator.txt"), customFont);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        blockedDomain.Update();
        blockedIp.Update();

        if (portButton.Update()) {
            proxy.setPort(std::stoi(portButton.GetInputText()));
        }

        connectionRecord.Update(proxy.connections);
        Usage.Update();
        
        int flag = startButton.Update() ;
        if (flag == 1) {
            // proxy_fd = proxy.start();
            proxy.start();
        } else if (flag == -1) {
            proxy.stop();
            // if (proxy_fd != INVALID_SOCKET) CLOSE_SOCKET(proxy_fd);            
        }

        // Draw
        BeginDrawing();
        ClearBackground(MY_BACKGROUND_COLOR);

        DrawTextEx(titleFont, "PROXY SERVER", {220, 10}, 100, 1, BLACK);
        Usage.Draw();
        dev.Draw();

        blockedDomain.Draw();
        blockedIp.Draw();
        portButton.Draw();
        startButton.Draw();


        connectionRecord.Draw();

        EndDrawing();
    }

    // Dọn dẹp tài nguyên
    UnloadFont(customFont);
    CloseWindow();
    return 0;
}
