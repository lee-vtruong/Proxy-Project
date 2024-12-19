#ifndef GUI_H
#define GUI_H

#include "raygui.h"
#include "raylib.h"
#include "raymath.h"
#include "http_parser.h"
#include "common_lib.h"

class Button {
protected:
    Rectangle bounds;         // Kích thước và vị trí của button
    Color baseColor;          // Màu cơ bản
    Color hoverColor;         // Màu khi hover
    Color pressColor;         // Màu khi nhấn
    Color textColor;          // Màu của text
    std::string text;         // Nội dung text hiển thị trên button
    int fontSize;             // Kích thước font chữ
    Font font;                // Font tùy chỉnh
    bool isHovered;           // Trạng thái hover
    bool isPressed;           // Trạng thái nhấn
    float cornerRadius;       // Bán kính bo góc

public:
    // Constructor
    Button(float x, float y, float width, float height, const std::string& buttonText, int textSize = 20, 
           float radius = 10.0f, Font customFont = GetFontDefault(), Color base = GRAY, Color hover = LIGHTGRAY, Color press = DARKGRAY, Color textCol = WHITE)
        : bounds{ x, y, width, height }, baseColor(base), hoverColor(hover), pressColor(press), textColor(textCol), text(buttonText),
          fontSize(textSize), font(customFont), isHovered(false), isPressed(false), cornerRadius(radius) {}

    // Kiểm tra trạng thái hover hoặc nhấn chuột
    bool Update() {
        Vector2 mousePoint = GetMousePosition();
        isHovered = CheckCollisionPointRec(mousePoint, bounds); // Kiểm tra chuột có hover lên button không
        isPressed = isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON); // Kiểm tra nhấn chuột trái
        return isPressed;
    }

    // Vẽ button lên màn hình (với bo góc)
    void Draw() {
        // Chọn màu dựa trên trạng thái
        Color currentColor = baseColor;
        if (isPressed) {
            currentColor = pressColor;
        } else if (isHovered) {
            currentColor = hoverColor;
        }

        // Vẽ hình chữ nhật bo góc của button
        DrawRectangleRounded(bounds, cornerRadius / bounds.width, 6, currentColor);

        // Vẽ viền button để làm đẹp hơn (tùy chọn)
        DrawRectangleRoundedLines(bounds, cornerRadius / bounds.width, 6, BLACK);

        // Vẽ text lên button (căn giữa)
        Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, 1);
        float textX = bounds.x + (bounds.width - textSize.x) / 2;
        float textY = bounds.y + (bounds.height - textSize.y) / 2;
        DrawTextEx(font, text.c_str(), { textX, textY }, fontSize, 1, textColor);
    }

    // Kiểm tra xem button có được nhấn không (dùng để thực hiện hành động)
    bool IsClicked() const {
        return isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    }

    // Setter cho text (nếu muốn cập nhật nội dung button)
    void SetText(const std::string& newText) {
        text = newText;
    }

    // Setter cho bán kính bo góc
    void SetCornerRadius(float radius) {
        cornerRadius = radius;
    }

    // Setter cho font
    void SetFont(Font customFont) {
        font = customFont;
    }
};


class ToggleButton : public Button {
private:
    std::string labelOn;   // Label khi button ở trạng thái "On"
    std::string labelOff;  // Label khi button ở trạng thái "Off"
    bool isOn;             // Trạng thái hiện tại của button

public:
    // Constructor
    ToggleButton(float x, float y, float width, float height, 
                 const std::string& textOn, const std::string& textOff, int textSize = 20, 
                 float radius = 10.0f, Font customFont = GetFontDefault(), Color base = GRAY, Color hover = LIGHTGRAY, Color press = DARKGRAY, Color textCol = WHITE)
        : Button(x, y, width, height, textOff, textSize, radius, customFont, base, hover, press, textCol), // Khởi tạo class cha
          labelOn(textOn), labelOff(textOff), isOn(false) {}

    // Ghi đè phương thức Update và xử lý trạng thái chuyển đổi
    int Update() {
        Button::Update();

        // Nếu button được nhấn, chuyển đổi trạng thái
        if (IsClicked()) {
            isOn = !isOn; // Đổi trạng thái
            if (!isOn) { //Hoi nguoc ngao xiu :")
                SetText(labelOff);
                return -1;
            } else {
                SetText(labelOn);
                return 1;
            }
        }
        return 0;
    }

    // Getter trạng thái
    bool GetState() const {
        return isOn;
    }
};


class TextBox {
private:
    Rectangle bounds;          // Kích thước và vị trí của TextBox
    Color borderColor;         // Màu viền khung
    Color backgroundColor;     // Màu nền
    Color textColor;           // Màu chữ
    Font font;                 // Font chữ
    int fontSize;              // Kích thước font
    float cornerRadius;        // Bán kính bo góc
    std::string text;          // Nội dung văn bản hiển thị
    float scrollOffset;        // Vị trí thanh cuộn
    float lineSpacing;         // Khoảng cách giữa các dòng
    int visibleLines;          // Số dòng có thể hiển thị trong khung
    int totalLines;            // Tổng số dòng văn bản
    std::vector<std::string> wrappedText; // Danh sách các dòng đã được "word-wrap"
    bool isDraggingScrollBar;  // Trạng thái kéo thanh cuộn
    float dragStartY;          // Điểm bắt đầu kéo chuột


    // Phương thức nội bộ để "word-wrap" văn bản
    void WrapText() {
        wrappedText.clear();
        std::istringstream stream(text);
        std::string line;
        float maxWidth = bounds.width - 20; // Trừ khoảng cách padding 10px mỗi bên

        while (std::getline(stream, line, '\n')) {
            std::string word;
            std::istringstream wordStream(line);
            std::string currentLine;

            while (wordStream >> word) {
                std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                Vector2 textSize = MeasureTextEx(font, testLine.c_str(), fontSize, 1);
                if (textSize.x > maxWidth) {
                    wrappedText.push_back(currentLine);
                    currentLine = word;
                } else {
                    currentLine = testLine;
                }
            }
            wrappedText.push_back(currentLine);
        }

        totalLines = wrappedText.size();
        visibleLines = bounds.height / (fontSize + lineSpacing);
    }

public:
    // Constructor
    TextBox(float x, float y, float width, float height, const std::string& content,
            Font customFont = GetFontDefault(), int textSize = 20, float radius = 10.0f,
            Color bg = LIGHTGRAY, Color border = GRAY, Color textCol = BLACK, float spacing = 5.0f)
        : bounds{ x, y, width, height }, borderColor(border), backgroundColor(bg),
          textColor(textCol), font(customFont), fontSize(textSize),
          cornerRadius(radius), text(content),
          scrollOffset(0), lineSpacing(spacing), visibleLines(0), totalLines(0),
          isDraggingScrollBar(false), dragStartY(0.0f) {
        WrapText();
    }

    // Cập nhật logic TextBox
    void Update() {
        // Xử lý con lăn chuột
        if (CheckCollisionPointRec(GetMousePosition(), bounds)) {
            float mouseWheelMove = GetMouseWheelMove();
            if (mouseWheelMove != 0.0f) {
                float scrollSpeed = 20.0f; // Tốc độ cuộn mỗi lần con lăn
                scrollOffset -= mouseWheelMove * scrollSpeed;
            }
            scrollOffset = Clamp(scrollOffset, 0.0f, (totalLines - visibleLines) * (fontSize + lineSpacing));
        }

        // Giới hạn thanh cuộn trong khoảng cho phép

        // Xử lý kéo thanh cuộn
        if (totalLines > visibleLines) {
            float scrollBarHeight = bounds.height * visibleLines / totalLines;
            Rectangle scrollBar = {
                bounds.x + bounds.width - 5,
                bounds.y + (bounds.height - scrollBarHeight) * (scrollOffset / ((totalLines - visibleLines) * (fontSize + lineSpacing))),
                5,
                scrollBarHeight
            };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), scrollBar)) {
                isDraggingScrollBar = true;
                dragStartY = GetMousePosition().y;
            }

            if (isDraggingScrollBar) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float deltaY = GetMousePosition().y - dragStartY;
                    dragStartY = GetMousePosition().y;

                    // Cập nhật scrollOffset dựa trên deltaY
                    float maxOffset = (totalLines - visibleLines) * (fontSize + lineSpacing);
                    scrollOffset += deltaY * (maxOffset / (bounds.height - scrollBarHeight));
                    scrollOffset = Clamp(scrollOffset, 0.0f, maxOffset);
                } else {
                    isDraggingScrollBar = false;
                }
            }
        }
    }

    // Vẽ TextBox lên màn hình
    void Draw() {
        // Vẽ khung bo góc
        DrawRectangleRounded(bounds, cornerRadius / bounds.width, 6, backgroundColor);
        DrawRectangleRoundedLines(bounds, cornerRadius / bounds.width, 6, borderColor);

        // Bắt đầu vùng cắt (scissor mode)
        BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);

        // Vẽ text (chỉ vẽ những dòng trong phạm vi hiển thị)
        float padding = 10.0f;
        float y = bounds.y + padding - scrollOffset;
        for (int i = 0; i < totalLines; i++) {
            if (y + fontSize > bounds.y + bounds.height) break; // Dừng nếu dòng vượt ra ngoài khung
            if (y + fontSize >= bounds.y) {                     // Chỉ vẽ nếu dòng nằm trong khung
                DrawTextEx(font, wrappedText[i].c_str(), { bounds.x + padding, y }, fontSize, 1, textColor);
            }
            y += fontSize + lineSpacing;
        }

        // Kết thúc vùng cắt
        EndScissorMode();

        // Vẽ thanh cuộn nếu cần
        if (totalLines > visibleLines) {
            float scrollBarHeight = bounds.height * visibleLines / totalLines;
            float scrollBarY = bounds.y + (bounds.height - scrollBarHeight) * (scrollOffset / ((totalLines - visibleLines) * (fontSize + lineSpacing)));

            DrawRectangle(bounds.x + bounds.width - 5, bounds.y, 5, bounds.height, DARKGRAY); // Nền thanh cuộn
            DrawRectangle(bounds.x + bounds.width - 5, scrollBarY, 5, scrollBarHeight, BLACK); // Thanh cuộn
        }
    }

    // Setter nội dung text
    void SetText(const std::string& newText) {
        text = newText;
        scrollOffset = 0;
        WrapText();
    }

    // Getter nội dung text
    std::string GetText() const {
        return text;
    }
};


class Popup {
private:
    Rectangle bounds;            // Vị trí và kích thước của Popup
    Color backgroundColor;       // Màu nền
    Color borderColor;           // Màu viền
    Color titleBarColor;         // Màu thanh tiêu đề
    Color textColor;             // Màu chữ
    std::string title;           // Tiêu đề popup
    TextBox contentBox;          // TextBox hiển thị nội dung
    float cornerRadius;          // Bán kính bo góc
    bool isDragging;             // Trạng thái kéo popup
    Vector2 dragOffset;          // Offset kéo chuột
    bool isVisible;              // Trạng thái hiển thị popup
    Font font;
public:
    // Constructor
    Popup(float x, float y, float width, float height, const std::string& titleText, const std::string& content,
          Font customFont = GetFontDefault(), int textSize = 20, float radius = 10.0f,
          Color bg = LIGHTGRAY, Color border = GRAY, Color titleColor = DARKGRAY, Color textCol = BLACK)
        : bounds{ x, y, width, height }, backgroundColor(bg), borderColor(border), titleBarColor(titleColor),
          textColor(textCol), title(titleText),
          contentBox(x + 10, y + 40, width - 20, height - 50, content, customFont, textSize, radius - 2, bg, border, textCol),
          cornerRadius(radius), isDragging(false), isVisible(true),
          font(customFont) {}

    // Cập nhật logic Popup
    bool Update() {
        if (!isVisible) return false;

        Vector2 mousePos = GetMousePosition();

        // Xử lý kéo di chuyển popup
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, { bounds.x, bounds.y, bounds.width, 30 })) {
            isDragging = true;
            dragOffset = { mousePos.x - bounds.x, mousePos.y - bounds.y };
        }

        if (isDragging) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                bounds.x = mousePos.x - dragOffset.x;
                bounds.y = mousePos.y - dragOffset.y;

                // Di chuyển TextBox cùng với Popup
                contentBox = TextBox(bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50, contentBox.GetText(), font);
            } else {
                isDragging = false;
            }
        }

        // Phím tắt đóng popup (ESC hoặc X góc phải)
        if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                                         CheckCollisionPointRec(mousePos, { bounds.x + bounds.width - 25, bounds.y + 5, 20, 20 }))) {
            isVisible = false;
            return false;
        }

        // Cập nhật TextBox
        contentBox.Update();
        return true;
    }

    // Vẽ Popup lên màn hình
    void Draw() {
        if (!isVisible) return;

        // Vẽ nền Popup
        DrawRectangleRounded(bounds, cornerRadius / bounds.width, 6, backgroundColor);
        DrawRectangleRoundedLines(bounds, cornerRadius / bounds.width, 6, borderColor);

        // Vẽ thanh tiêu đề
        DrawRectangleRounded({ bounds.x, bounds.y, bounds.width, 30 }, cornerRadius / bounds.width, 6, titleBarColor);
        DrawTextEx(font, title.c_str(), {bounds.x + 10, bounds.y + 7}, 20, 6, textColor);

        // Vẽ nút đóng (X)
        DrawTextEx(font, "X", {bounds.x + bounds.width - 20, bounds.y + 5}, 20, 6, RED);

        // Vẽ TextBox nội dung
        contentBox.Draw();
    }

    // Hiển thị hoặc ẩn Popup
    void SetVisible(bool visible) {
        isVisible = visible;
    }

    // Kiểm tra trạng thái hiển thị
    bool IsVisible() const {
        return isVisible;
    }

    // Đặt nội dung Popup
    void SetContent(const std::string& content) {
        contentBox.SetText(content);
    }
    
    Rectangle GetBounds() const {
        return bounds;
    }
};


class Table {
private:
    Rectangle bounds;                         // Kích thước và vị trí của bảng
    std::vector<ConnectionInfo> data;         // Dữ liệu hiển thị
    Font font;                                // Font chữ
    int fontSize;                             // Kích thước font
    float rowHeight;                          // Chiều cao mỗi hàng
    float scrollOffset;                       // Offset thanh cuộn
    int selectedRow;                          // Hàng được chọn (-1 nếu không có hàng nào)
    float lineSpacing;                        // Khoảng cách giữa các dòng
    bool isDraggingScrollBar;                 // Trạng thái kéo thanh cuộn
    float dragStartY;                         // Điểm bắt đầu kéo chuột
    Popup* detailPopup;                       // Popup chi tiết (nullptr nếu không hiển thị)
    int visibleLines;          // Số dòng có thể hiển thị trong khung
    int totalLines; 
    Rectangle bounds_d;

    // Vẽ một hàng
    void DrawRow(int rowIndex, float y, bool isHovered, bool isSelected) {
        // Màu nền hàng khi được hover hoặc chọn
        Color backgroundColor = isSelected ? DARKGRAY : (isHovered ? LIGHTGRAY : WHITE);
        DrawRectangle(bounds.x, y, bounds.width, rowHeight, backgroundColor);

        const ConnectionInfo& connection = data[rowIndex];
        std::ostringstream oss;
        oss << connection.transactions[0].request.method;  // Method
        std::string method = oss.str();

        std::string clientIp = connection.client.ip;       // Client IP
        std::string serverIp = connection.server.ip;       // Server IP
        std::string url = connection.transactions[0].request.url; // URL

        // Vẽ dữ liệu từng cột
        float columnX[] = { bounds.x + 10, bounds.x + 120, bounds.x + 300, bounds.x + 500 };
        DrawTextEx(font, method.c_str(), { columnX[0], y + 5 }, fontSize, 1, BLACK);
        DrawTextEx(font, clientIp.c_str(), { columnX[1], y + 5 }, fontSize, 1, BLACK);
        DrawTextEx(font, serverIp.c_str(), { columnX[2], y + 5 }, fontSize, 1, BLACK);
        DrawTextEx(font, url.c_str(), { columnX[3], y + 5 }, fontSize, 1, BLACK);
    }

    void WrapText() {
        totalLines = data.size();
        visibleLines = bounds.height / (fontSize + lineSpacing);
    }

public:
    // Constructor
    Table(float x, float y, float width, float height, const std::vector<ConnectionInfo>& connectionData,
          Font customFont = GetFontDefault(), int textSize = 20, float rowSpacing = 5.0f)
        : bounds{ x, y, width, height }, data(connectionData), font(customFont), fontSize(textSize),
          rowHeight(textSize + rowSpacing), scrollOffset(0), selectedRow(-1), lineSpacing(rowSpacing),
          isDraggingScrollBar(false), dragStartY(0.0f), detailPopup(nullptr), visibleLines(0), totalLines(0),
          bounds_d{x, y + rowHeight, width, height - rowHeight} {
            WrapText();
          }

    // Cập nhật logic bảng
    void Update(std::vector<ConnectionInfo>& connections) {
        // data = std::vector<ConnectionInfo>((connections.size() > 50) ?  connections.end() - 50 : connections.begin(), connections.end());
        data = connections;
        WrapText();
        Vector2 mousePosition = GetMousePosition();
        // Xử lý cuộn chuột khi chuột nằm trong bảng và không trong popup
        if (CheckCollisionPointRec(mousePosition, bounds) && !detailPopup) {
            float mouseWheelMove = GetMouseWheelMove();
            if (mouseWheelMove != 0.0f) {
                float scrollSpeed = rowHeight; // Tốc độ cuộn
                scrollOffset -= mouseWheelMove * scrollSpeed;
            }

            // Giới hạn thanh cuộn trong khoảng cho phép
            float maxScroll = std::max(0.0f, (data.size() + 1) * rowHeight - bounds.height);
            scrollOffset = Clamp(scrollOffset, 0.0f, maxScroll);
        }
                // Xử lý kéo thanh cuộn
        float scrollBarHeight = bounds.height * visibleLines / totalLines;
        Rectangle scrollBar = {
            bounds.x + bounds.width - 5,
            bounds.y + (bounds.height - scrollBarHeight) * (scrollOffset / ((totalLines - visibleLines) * (fontSize + lineSpacing))),
            5,
            scrollBarHeight
        };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, scrollBar)) {
            isDraggingScrollBar = true;
            dragStartY = mousePosition.y;
        }

        if (isDraggingScrollBar) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float deltaY = mousePosition.y - dragStartY;
                dragStartY = mousePosition.y;

                // Cập nhật scrollOffset dựa trên deltaY
                float maxOffset = (totalLines - visibleLines) * (fontSize + lineSpacing);
                scrollOffset += deltaY * (maxOffset / (bounds.height - scrollBarHeight));
                scrollOffset = Clamp(scrollOffset, 0.0f, maxOffset);
            } else {
                isDraggingScrollBar = false;
            }
        }

        // Xử lý logic popup nếu có
        if (detailPopup) {
            if(!detailPopup->Update()) {
                delete detailPopup;
                detailPopup = nullptr;
            }
        }

        // Xử lý chọn hàng khi chuột nằm trong bảng
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePosition, bounds_d) && !detailPopup) {
                int hoveredRow = (mousePosition.y - bounds_d.y + scrollOffset) / rowHeight;
                if (hoveredRow >= 0 && hoveredRow < (int)data.size()) {
                    selectedRow = hoveredRow;
                    // std::cout << "Selected row: " << selectedRow << std::endl;
                    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            } else {
                selectedRow = -1; // Bỏ chọn nếu nhấp ra ngoài bảng
            }
        }

        // Xử lý nhấp đúp để hiển thị chi tiết
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && selectedRow != -1) {
            static double lastClickTime = 0.0;
            double currentTime = GetTime();
            if (currentTime - lastClickTime < 0.3) { // Nhấp đúp
                if (detailPopup == nullptr) {
                    std::string connectionDetails = ConnectionInfoToString(data[selectedRow]);
                    detailPopup = new Popup(bounds.x + bounds.width / 2 - 250, bounds.y + bounds.height / 2 - 150, 500, 300, "Connection Details", connectionDetails, font);
                }
            }
            lastClickTime = currentTime;
        }

        // Đóng popup nếu nhấp ra ngoài popup
        if (detailPopup && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!CheckCollisionPointRec(mousePosition, bounds) && !CheckCollisionPointRec(mousePosition, detailPopup->GetBounds())) {
                delete detailPopup;
                detailPopup = nullptr;
            }
        }
    }

    // Vẽ bảng lên màn hình
    void Draw() {
        // Vẽ nền bảng
        DrawRectangleRounded(bounds, 0.01, 10, LIGHTGRAY);
        DrawRectangleRoundedLinesEx(bounds, 0.01, 10, 2, DARKGRAY);

        // Vẽ tiêu đề các cột
        float columnX[] = { bounds.x + 10, bounds.x + 120, bounds.x + 300, bounds.x + 500 };
        float headerY = bounds.y; // Vị trí Y của tiêu đề

        DrawRectangle(bounds.x, headerY, bounds.width, rowHeight, DARKGRAY); // Nền tiêu đề
        DrawTextEx(font, "Method", { columnX[0], headerY + 5 }, fontSize, 1, WHITE);
        DrawTextEx(font, "IP Source", { columnX[1], headerY + 5 }, fontSize, 1, WHITE);
        DrawTextEx(font, "IP Remote", { columnX[2], headerY + 5 }, fontSize, 1, WHITE);
        DrawTextEx(font, "URL", { columnX[3], headerY + 5 }, fontSize, 1, WHITE);

        // Bắt đầu vùng cắt
        BeginScissorMode(bounds.x, bounds.y + rowHeight, bounds.width, bounds.height - rowHeight);

        // Vẽ các hàng dữ liệu
        float y = bounds.y + rowHeight - scrollOffset; // Dòng đầu tiên dưới tiêu đề
        for (size_t i = 0; i < data.size(); ++i) {
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), { bounds.x, y, bounds.width, rowHeight });
            DrawRow(i, y, isHovered, false);
            y += rowHeight;
        }

        // Kết thúc vùng cắt
        EndScissorMode();

        // Vẽ thanh cuộn
        float maxScroll = std::max(0.0f, (data.size() + 1) * rowHeight - bounds.height);
        if (maxScroll > 0) {
            float scrollBarHeight = (bounds.height - rowHeight) * (bounds.height - rowHeight) / (data.size() * rowHeight);
            float scrollBarY = bounds.y + rowHeight + ((bounds.height - rowHeight - scrollBarHeight) * (scrollOffset / maxScroll));
            DrawRectangle(bounds.x + bounds.width - 5, bounds.y + rowHeight, 5, bounds.height - rowHeight, DARKGRAY); // Nền thanh cuộn
            DrawRectangle(bounds.x + bounds.width - 5, scrollBarY, 5, scrollBarHeight, BLACK); // Thanh cuộn
        }

        // Vẽ Popup chi tiết nếu có
        if (detailPopup) {
            detailPopup->Draw();
        }
    }
};

// Class InputText (thanh tìm kiếm cải tiến)
class InputText {
private:
    Rectangle bounds;           // Vị trí và kích thước của InputText
    int maxChars;               // Giới hạn số ký tự nhập
    Color baseColor;            // Màu nền
    Color borderColor;          // Màu viền
    Color textColor;            // Màu chữ
    Color cursorColor;          // Màu con trỏ
    bool isActive;              // Trạng thái đang nhập
    int cursorPosition;         // Vị trí con trỏ
    double blinkTime;           // Thời gian nhấp nháy con trỏ
    float textOffsetX;          // Khoảng dịch văn bản
    double backspaceTimer;      // Bộ đếm thời gian giữ Backspace
    Font font;                  // Font chữ
    int fontSize;               // Kích thước font
    std::string text;           // Nội dung văn bản

public:
    // Constructor
    InputText(float x, float y, float width, float height, int maxLength = 64, Font customFont = GetFontDefault(), int textSize = 20)
        : bounds{ x, y, width, height }, maxChars(maxLength), baseColor{ LIGHTGRAY }, borderColor{ DARKGRAY },
          textColor{ BLACK }, cursorColor{ BLACK }, isActive(false), cursorPosition(0), blinkTime(0.0),
          textOffsetX(0), backspaceTimer(0.0), font(customFont), fontSize(textSize), text("") {}

    // Cập nhật logic InputText
    void Update() {
        Vector2 mousePoint = GetMousePosition();

        // Kích hoạt vùng nhập liệu khi click chuột
        if (CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isActive = true;
        } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isActive = false;
        }

        // Nếu đang nhập liệu
        if (isActive) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && ((int)text.length() < maxChars)) {
                    text.insert(cursorPosition, 1, (char)key);
                    cursorPosition++;
                    AdjustTextOffset();
                }
                key = GetCharPressed();
            }

            // Xóa ký tự khi giữ Backspace
            if (IsKeyDown(KEY_BACKSPACE)) {
                backspaceTimer += GetFrameTime();
                if (cursorPosition > 0 && (backspaceTimer > 0.5 || IsKeyPressed(KEY_BACKSPACE))) {
                    text.erase(cursorPosition - 1, 1);
                    cursorPosition--;
                    backspaceTimer = (backspaceTimer > 0.5) ? 0.05 : 0.0; // Tăng tốc xóa sau giữ lâu
                    AdjustTextOffset();
                }
            } else {
                backspaceTimer = 0.0;
            }

            // Di chuyển con trỏ
            if (IsKeyPressed(KEY_LEFT) && cursorPosition > 0) {
                cursorPosition--;
                AdjustTextOffset();
            }
            if (IsKeyPressed(KEY_RIGHT) && cursorPosition < (int)text.length()) {
                cursorPosition++;
                AdjustTextOffset();
            }
        }

        // Nhấp nháy con trỏ
        blinkTime += GetFrameTime();
    }

    // Điều chỉnh khoảng dịch văn bản để con trỏ luôn nằm trong vùng hiển thị
    void AdjustTextOffset() {
        float cursorX = MeasureTextEx(font, text.substr(0, cursorPosition).c_str(), fontSize, 1).x;

        // Đảm bảo con trỏ luôn nằm trong vùng hiển thị
        if (cursorX - textOffsetX > bounds.width - 20) {
            textOffsetX = cursorX - (bounds.width - 20);
        } else if (cursorX - textOffsetX < 0) {
            textOffsetX = cursorX;
        }

        if (textOffsetX < 0) textOffsetX = 0;
    }

    // Vẽ InputText
    void Draw() {
        // Vẽ vùng nhập liệu bo góc
        DrawRectangleRounded(bounds, 0.2, 10, baseColor);
        DrawRectangleRoundedLinesEx(bounds, 0.2, 10, 2, borderColor);

        // Hiển thị văn bản
        Rectangle clip = { bounds.x + 10, bounds.y, bounds.width - 20, bounds.height };
        BeginScissorMode(clip.x, clip.y, clip.width, clip.height);

        float textX = bounds.x + 10 - textOffsetX;
        float textY = bounds.y + (bounds.height - fontSize) / 2;
        DrawTextEx(font, text.c_str(), { textX, textY }, fontSize, 1, textColor);

        EndScissorMode();

        // Vẽ con trỏ nhấp nháy
        if (isActive && ((int)(blinkTime * 2) % 2 == 0)) {
            float cursorX = textX + MeasureTextEx(font, text.substr(0, cursorPosition).c_str(), fontSize, 1).x;
            DrawLineV({ cursorX, textY }, { cursorX, textY + fontSize }, cursorColor);
        }
    }

    // Getter văn bản
    std::string GetText() const {
        return text;
    }

    // Setter văn bản
    void SetText(const std::string& newText) {
        text = newText;
        cursorPosition = text.length();
        textOffsetX = 0;
    }
};

// Class InputFieldWithButton (kết hợp InputText và Button)
class InputFieldWithButton {
private:
    InputText inputField;
    Button actionButton;

public:
    // Constructor
    InputFieldWithButton(float inputX, float inputY, float inputWidth, float inputHeight,
                         const std::string& buttonText, float buttonX, float buttonY, float buttonWidth, float buttonHeight, Font font = GetFontDefault())
        : inputField(inputX, inputY, inputWidth, inputHeight, 256, font),
          actionButton(buttonX, buttonY, buttonWidth, buttonHeight, buttonText, 20.f, 10.f, font) {}

    // Cập nhật logic
    bool Update() {
        inputField.Update();
        return actionButton.Update();
    }

    // Vẽ lên màn hình
    void Draw() {
        inputField.Draw();
        actionButton.Draw();
    }

    // Kiểm tra button có được nhấn không
    bool IsButtonClicked() const {
        return actionButton.IsClicked();
    }

    // Getter văn bản từ InputText
    std::string GetInputText() const {
        return inputField.GetText();
    }

    void clear() {
        inputField.SetText("");
    }

    void SetText(const std::string& newText) {
        inputField.SetText(newText);
    }
};


class NameList {
private:
    std::string fileName;
    Rectangle bounds;
    std::unordered_set<std::string>& nameSet;
    Font font;
    
    InputFieldWithButton inputFieldWithButton; 
    bool showContextMenu;
    Vector2 contextMenuPosition;


    int selectedNameIndex;
    float scrollOffset; // Biến để xử lý scrolling
    std::string title; // Tiêu đề của bảng
    std::vector<std::string> nameVector;
    const float rowHeight = 30; // Chiều cao cố định cho mỗi dòng
    int visibleRows; // Số hàng hiển thị tối đa trong khung
    Rectangle bounds_d;
    int fontSize;
    int lineSpacing;
public:
    // Constructor (thêm tham số title cho tiêu đề)
    NameList(std::string filename, float x, float y, float width, float height, 
             std::unordered_set<std::string>& names, Font customFont, const std::string& titleText, int textSize = 20, int rowSpacing = 5.0f)
        : fileName(filename), bounds{x, y, width, height - 50}, nameSet(names), font(customFont),
          inputFieldWithButton(x + 10, y + height - 40, width - 120, 30, "Add", x + width - 100, y + height - 40, 90, 30, customFont),
          showContextMenu(false), contextMenuPosition{0, 0}, selectedNameIndex(-1), scrollOffset(0.0f), title(titleText),
          bounds_d{x, y + rowHeight, width, height - rowHeight - 50}, fontSize(textSize), lineSpacing(rowSpacing) {
        UpdateNameVector();
        visibleRows = bounds.height / (fontSize + lineSpacing);
    }

    // Cập nhật logic
    void Update() {
        UpdateNameVector();
        Vector2 mousePoint = GetMousePosition();

        // Xử lý cuộn chuột
        if (CheckCollisionPointRec(mousePoint, bounds)) {
            float mouseWheelMove = GetMouseWheelMove();
            if (mouseWheelMove != 0.0f) {
                float scrollSpeed = rowHeight; // Tốc độ cuộn
                scrollOffset -= mouseWheelMove * scrollSpeed;
            }

            // Giới hạn thanh cuộn trong khoảng cho phép
            float maxScroll = std::max(0.0f, (nameVector.size() + 1) * rowHeight - bounds.height);
            scrollOffset = Clamp(scrollOffset, 0.0f, maxScroll);
        }

        // Xử lý thanh cuộn kéo
        HandleScrollBar(mousePoint);

        // Xử lý input text field
        inputFieldWithButton.Update();
        if (inputFieldWithButton.IsButtonClicked()) {
            std::string newName = inputFieldWithButton.GetInputText();
            if (!newName.empty() && nameSet.find(newName) == nameSet.end()) {
                nameSet.insert(newName);
                inputFieldWithButton.clear();
                SaveToFile();
                UpdateNameVector();
            }
        }

        // Xử lý context menu
        HandleContextMenu(mousePoint);
    }

    // Xử lý cuộn chuột và kéo thanh cuộn
    void HandleScrollBar(Vector2 mousePosition) {
        // if ((int)nameVector.size() <= visibleRows) return;
        // float maxScroll = std::max(0.0f, rowHeight * (nameVector.size() - visibleRows));

        // Xử lý kéo thanh cuộn
        float scrollBarHeight = (bounds.height * visibleRows) / nameVector.size();
        Rectangle scrollBar = { bounds.x + bounds.width - 5,
            bounds.y + (bounds.height - scrollBarHeight) * (scrollOffset / ((nameVector.size() - visibleRows) * (fontSize + lineSpacing))), 
            5, 
            scrollBarHeight 
        };

        // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, scrollBar)) {
        //     float dragStartY = mousePoint.y;
        //     while (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        //         float deltaY = mousePoint.y - dragStartY;
        //         dragStartY = mousePoint.y;

        //         scrollOffset += deltaY * maxScroll / (bounds.height - scrollBarHeight);
        //         scrollOffset = Clamp(scrollOffset, 0.0f, maxScroll);
        //     }
        // }
        bool isDraggingScrollBar = false;
        float dragStartY = mousePosition.y;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, scrollBar)) {
            isDraggingScrollBar = true;
            dragStartY = mousePosition.y;
        }

        if (isDraggingScrollBar) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float deltaY = mousePosition.y - dragStartY;
                dragStartY = mousePosition.y;

                // Cập nhật scrollOffset dựa trên deltaY
                float maxOffset = (nameVector.size() - visibleRows) * (fontSize + lineSpacing);
                scrollOffset += deltaY * (maxOffset / (bounds.height - scrollBarHeight));
                scrollOffset = Clamp(scrollOffset, 0.0f, maxOffset);
            } else {
                isDraggingScrollBar = false;
            }
        }
    }

    // Xử lý menu chuột phải
    void HandleContextMenu(Vector2 mousePoint) {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            for (int i = 0; i < visibleRows; i++) {
                int index = i + static_cast<int>(scrollOffset / rowHeight);
                if (index >= (int)nameVector.size()) break;

                float yPosition = bounds_d.y + i * rowHeight;
                Rectangle nameBounds = {bounds_d.x, yPosition, bounds.width, rowHeight};
                if (CheckCollisionPointRec(mousePoint, nameBounds)) {
                    showContextMenu = true;
                    contextMenuPosition = mousePoint;
                    selectedNameIndex = index;
                    return;
                }
            }
        }

        // Xóa tên nếu chọn "Delete"
        if (showContextMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Rectangle deleteOption = {contextMenuPosition.x, contextMenuPosition.y, 80, 30};
            if (CheckCollisionPointRec(mousePoint, deleteOption)) {
                nameSet.erase(nameVector[selectedNameIndex]);
                SaveToFile();
                UpdateNameVector();
                showContextMenu = false;
            } else {
                showContextMenu = false; // Ẩn menu nếu click ra ngoài
            }
        }
    }

    void Draw() {
        // Vẽ khung danh sách
        DrawRectangleRounded(bounds, 0.01, 10, LIGHTGRAY);
        DrawRectangleRoundedLinesEx(bounds, 0.01, 10, 2, DARKGRAY);

        // Vẽ tiêu đề
        DrawRectangle(bounds.x, bounds.y, bounds.width, rowHeight, DARKGRAY); // Nền tiêu đề
        DrawTextEx(font, title.c_str(), {bounds.x + 10, bounds.y + 5}, 20, 1, WHITE);

        // Vùng cắt để giới hạn vẽ trong khung
        BeginScissorMode(bounds.x, bounds.y + rowHeight, bounds.width, bounds.height - rowHeight);

        // Vẽ các hàng dữ liệu
        float yPosition = bounds.y + rowHeight - scrollOffset;
        for (size_t index = 0; index < nameVector.size(); index++) {
            Rectangle nameBounds = {bounds.x, yPosition, bounds.width, rowHeight};
            Color hoverColor = CheckCollisionPointRec(GetMousePosition(), nameBounds) ? DARKGRAY : WHITE;

            DrawRectangleRec(nameBounds, hoverColor);
            DrawTextEx(font, nameVector[index].c_str(), {nameBounds.x + 10, yPosition + 5}, 20, 1, BLACK);
            yPosition += rowHeight;
        }

        // if ((int)nameVector.size() > visibleRows) {
        //     float maxScroll = rowHeight * (nameVector.size() - visibleRows);
        //     float scrollBarHeight = (bounds.height - rowHeight) * visibleRows / nameVector.size();
        //     float scrollBarY = bounds.y + rowHeight + (scrollOffset / maxScroll) * (bounds.height - rowHeight - scrollBarHeight);

        //     // Vẽ thanh cuộn nền và phần cuộn hiện tại
        //     DrawRectangle(bounds.x + bounds.width - 10, bounds.y + rowHeight, 10, bounds.height - rowHeight, LIGHTGRAY);
        //     DrawRectangle(bounds.x + bounds.width - 10, scrollBarY, 10, scrollBarHeight, GRAY);
        // }

        EndScissorMode();

        // Vẽ thanh cuộn
        float maxScroll = std::max(0.0f, (nameVector.size() + 1) * rowHeight - bounds.height);
        if (maxScroll > 0) {
            float scrollBarHeight = (bounds.height - rowHeight) * (bounds.height - rowHeight) / (nameVector.size() * rowHeight);
            float scrollBarY = bounds.y + rowHeight + ((bounds.height - rowHeight - scrollBarHeight) * (scrollOffset / maxScroll));
            DrawRectangle(bounds.x + bounds.width - 5, bounds.y + rowHeight, 5, bounds.height - rowHeight, DARKGRAY); // Nền thanh cuộn
            DrawRectangle(bounds.x + bounds.width - 5, scrollBarY, 5, scrollBarHeight, BLACK); // Thanh cuộn
        }

        // Vẽ input field và button
        inputFieldWithButton.Draw();

        // Vẽ menu "Delete"
        if (showContextMenu) {
            DrawRectangleRec({contextMenuPosition.x, contextMenuPosition.y, 80, 30}, RED);
            DrawTextEx(font, "Delete", {contextMenuPosition.x + 10, contextMenuPosition.y + 5}, 20, 1, WHITE);
        }
    }

    // Lưu danh sách vào file
    void SaveToFile() {
        std::ofstream outFile(fileName);
        for (const auto& name : nameSet) {
            outFile << name << "\n";
        }
        outFile.close();
    }

    // Cập nhật vector từ unordered_set
    void UpdateNameVector() {
        nameVector = std::vector<std::string>(nameSet.begin(), nameSet.end());
        std::sort(nameVector.begin(), nameVector.end());
    }
};

#endif // GUI_H