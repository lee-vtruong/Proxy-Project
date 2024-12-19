#include "../include/GUI.h"
#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include <sstream>
#include <string>
#include <vector>
#include "../include/proxy.h"


int main() {
    InitWindow(1600, 900, "Proxy Server");

    Font customFont = LoadFont("asset/Roboto.ttf");

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

    InputFieldWithButton portButton(150, 125, 100, 50, "Change Port", 300, 125, 150, 50, customFont);
    portButton.SetText(std::to_string(proxy.getPort()));

    ToggleButton startButton(550, 125, 200, 50, "Stop Proxy", "Start Proxy", 20, 10.0f, customFont, GRAY, LIGHTGRAY, DARKGRAY, WHITE);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        blockedDomain.Update();
        blockedIp.Update();

        if (portButton.Update()) {
            proxy.setPort(std::stoi(portButton.GetInputText()));
        }

        connectionRecord.Update(proxy.connections);
        
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
        ClearBackground(RAYWHITE);

        DrawTextEx(customFont, "PROXY SERVER", {220, 15}, 70, 1, GRAY);
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
