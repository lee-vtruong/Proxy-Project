
#include "../include/gui.h"

// --------------------------- Button Class ---------------------------
Button::Button(float x, float y, float width, float height, const std::string& buttonText,
               int textSize, float radius, Font customFont, Color base, Color hover, 
               Color press, Color textCol, Color hoverText)
    : bounds{ x, y, width, height }, baseColor(base), hoverColor(hover), pressColor(press), textColor(textCol),
      hoverTextColor(hoverText), text(buttonText), fontSize(textSize), font(customFont), isHovered(false),
      isPressed(false), cornerRadius(radius) {}

bool Button::Update() {
    Vector2 mousePoint = GetMousePosition();
    isHovered = CheckCollisionPointRec(mousePoint, bounds); 
    isPressed = isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON); 
    return isPressed;
}

void Button::Draw() {
    Color currentColor = baseColor, currentTextColor = textColor;
    if (isPressed) {
        currentColor = pressColor;
        currentTextColor = hoverTextColor;
    } else if (isHovered) {
        currentColor = hoverColor;
        currentTextColor = hoverTextColor;
    }

    DrawRectangleRounded(bounds, cornerRadius / bounds.width, 6, currentColor);
    DrawRectangleRoundedLines(bounds, cornerRadius / bounds.width, 6, BLACK);

    Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, 1);
    float textX = bounds.x + (bounds.width - textSize.x) / 2;
    float textY = bounds.y + (bounds.height - textSize.y) / 2;
    DrawTextEx(font, text.c_str(), { textX, textY }, fontSize, 1, currentTextColor);
}

bool Button::IsClicked() const {
    return isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

void Button::SetText(const std::string& newText) {
    text = newText;
}

void Button::SetCornerRadius(float radius) {
    cornerRadius = radius;
}

void Button::SetFont(Font customFont) {
    font = customFont;
}

// --------------------------- ToggleButton Class ---------------------------
ToggleButton::ToggleButton(float x, float y, float width, float height, 
                const std::string& textOn, const std::string& textOff, int textSize, 
                float radius, Font customFont, Color base, Color hover, 
                Color press, Color textCol, Color hoverText)
    : Button(x, y, width, height, textOff, textSize, radius, customFont, base, hover, press, textCol, hoverText),
      labelOn(textOn), labelOff(textOff), isOn(false) {}

int ToggleButton::Update() {
    Button::Update();

    if (IsClicked()) {
        isOn = !isOn; 
        if (!isOn) { 
            SetText(labelOff);
            return -1;
        } else {
            SetText(labelOn);
            return 1;
        }
    }
    return 0;
}

bool ToggleButton::GetState() const {
    return isOn;
}

// --------------------------- TextBox Class ---------------------------
TextBox::TextBox(float x, float y, float width, float height, const std::string& content,
                 Font customFont, int textSize, float radius, Color bg, 
                 Color border, Color textCol, float spacing)
    : bounds{ x, y, width, height }, borderColor(border), backgroundColor(bg),
      textColor(textCol), font(customFont), fontSize(textSize),
      cornerRadius(radius), text(content),
      scrollOffset(0), lineSpacing(spacing), visibleLines(0), totalLines(0),
      isDraggingScrollBar(false), dragStartY(0.0f) { WrapText(); }

void TextBox::WrapText() {
    wrappedText.clear();
    std::istringstream stream(text);
    std::string line;
    float maxWidth = bounds.width - 20; 

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

void TextBox::Update() {
    if (CheckCollisionPointRec(GetMousePosition(), bounds)) {
        float mouseWheelMove = GetMouseWheelMove();
        if (mouseWheelMove != 0.0f) {
            float scrollSpeed = 20.0f; 
            scrollOffset -= mouseWheelMove * scrollSpeed;
        }
        scrollOffset = Clamp(scrollOffset, 0.0f, (totalLines - visibleLines) * (fontSize + lineSpacing));
    }

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
                float maxOffset = (totalLines - visibleLines) * (fontSize + lineSpacing);
                scrollOffset += deltaY * (maxOffset / (bounds.height - scrollBarHeight));
                scrollOffset = Clamp(scrollOffset, 0.0f, maxOffset);
            } else {
                isDraggingScrollBar = false;
            }
        }
    }
}

void TextBox::Draw() {
    DrawRectangleRounded(bounds, cornerRadius / bounds.width, 6, backgroundColor);
    DrawRectangleRoundedLines(bounds, cornerRadius / bounds.width, 6, borderColor);

    BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);

    float padding = 10.0f;
    float y = bounds.y + padding - scrollOffset;
    for (int i = 0; i < totalLines; i++) {
        if (y + fontSize > bounds.y + bounds.height) break; 
        if (y + fontSize >= bounds.y) {                    
            DrawTextEx(font, wrappedText[i].c_str(), { bounds.x + padding, y }, fontSize, 1, textColor);
        }
        y += fontSize + lineSpacing;
    }

    EndScissorMode();

    if (totalLines > visibleLines) {
        float scrollBarHeight = bounds.height * visibleLines / totalLines;
        float scrollBarY = bounds.y + (bounds.height - scrollBarHeight) * (scrollOffset / ((totalLines - visibleLines) * (fontSize + lineSpacing)));

        DrawRectangle(bounds.x + bounds.width - 5, bounds.y, 5, bounds.height, DARKGRAY);
        DrawRectangle(bounds.x + bounds.width - 5, scrollBarY, 5, scrollBarHeight, BLACK);
    }
}

void TextBox::SetText(const std::string& newText) {
    text = newText;
    scrollOffset = 0;
    WrapText();
}

std::string TextBox::GetText() const {
    return text;
}


// --------------------------- Popup Class ---------------------------
Popup::Popup(float x, float y, float width, float height, const std::string& titleText,
             const std::string& content, Font customFont, int textSize, float radius, 
             Color bg, Color border, Color titleColor, Color textCol)
    : bounds{ x, y, width, height }, backgroundColor(bg), borderColor(border),
      titleBarColor(titleColor), textColor(textCol), title(titleText),
      contentBox(x + 10, y + 40, width - 20, height - 50, content, customFont, textSize, radius - 2, bg, border, textCol),
      cornerRadius(radius), isDragging(false), isVisible(true), font(customFont) {}

bool Popup::Update() {
    if (!isVisible) return false;


    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, { bounds.x, bounds.y, bounds.width, 30 })) {
        isDragging = true;
        dragOffset = { mousePos.x - bounds.x, mousePos.y - bounds.y };
    }

    if (isDragging) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            bounds.x = mousePos.x - dragOffset.x;
            bounds.y = mousePos.y - dragOffset.y;
            contentBox = TextBox(bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50, contentBox.GetText(), font);
        } else {
            isDragging = false;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(mousePos, { bounds.x + bounds.width - 25, bounds.y + 5, 20, 20 }))) {
        isVisible = false;
        return false;
    }

    contentBox.Update();
    return true;
}

void Popup::Draw() {
    if (!isVisible) return;

    DrawRectangleRounded(bounds, cornerRadius / bounds.width, 6, backgroundColor);
    DrawRectangleRoundedLines(bounds, cornerRadius / bounds.width, 6, borderColor);

    DrawRectangleRounded({ bounds.x, bounds.y, bounds.width, 30 }, cornerRadius / bounds.width, 6, titleBarColor);
    DrawTextEx(font, title.c_str(), {bounds.x + 10, bounds.y + 7}, 20, 6, WHITE);

    DrawTextEx(font, "X", {bounds.x + bounds.width - 20, bounds.y + 5}, 20, 6, RED);

    contentBox.Draw();
}

void Popup::SetVisible(bool visible) {
    isVisible = visible;
}

bool Popup::IsVisible() const {
    return isVisible;
}

void Popup::SetContent(const std::string& content) {
    contentBox.SetText(content);
}

Rectangle Popup::GetBounds() const {
    return bounds;
}

// --------------------------- Table Class ---------------------------
Table::Table(float x, float y, float width, float height, const std::vector<ConnectionInfo>& connectionData,
             Font customFont, int textSize, float rowSpacing)
    : bounds{ x, y, width, height }, data(connectionData), font(customFont), fontSize(textSize),
      rowHeight(textSize + rowSpacing), scrollOffset(0), selectedRow(-1), lineSpacing(rowSpacing),
      isDraggingScrollBar(false), dragStartY(0.0f), detailPopup(nullptr), visibleLines(0), totalLines(0),
      bounds_d{x, y + rowHeight, width, height - rowHeight} { WrapText(); }

void Table::DrawRow(int rowIndex, float y, bool isHovered, bool isSelected) {
    Color backgroundColor = isSelected ? DARKGRAY : (isHovered ? LIGHTGRAY : FILLED_COLOR);
    DrawRectangle(bounds.x, y, bounds.width, rowHeight, backgroundColor);

    const ConnectionInfo& connection = data[rowIndex];
    std::ostringstream oss;
    oss << connection.transactions[0].request.method; 
    std::string method = oss.str();

    std::string clientIp = connection.client.ip;     
    std::string serverIp = connection.server.ip;      
    std::string url = connection.transactions[0].request.url; 

    float columnX[] = { bounds.x + 10, bounds.x + 120, bounds.x + 300, bounds.x + 500 };
    DrawTextEx(font, method.c_str(), { columnX[0], y + 5 }, fontSize, 1, BLACK);
    DrawTextEx(font, clientIp.c_str(), { columnX[1], y + 5 }, fontSize, 1, BLACK);
    DrawTextEx(font, serverIp.c_str(), { columnX[2], y + 5 }, fontSize, 1, BLACK);
    DrawTextEx(font, url.c_str(), { columnX[3], y + 5 }, fontSize, 1, BLACK);
}

void Table::WrapText() {
    totalLines = data.size();
    visibleLines = bounds.height / (fontSize + lineSpacing);
}

void Table::Update(std::vector<ConnectionInfo>& connections) {
    data = connections;
    WrapText();

    Vector2 mousePosition = GetMousePosition();
    if (CheckCollisionPointRec(mousePosition, bounds) && !detailPopup) {
        float mouseWheelMove = GetMouseWheelMove();
        if (mouseWheelMove != 0.0f) {
            float scrollSpeed = rowHeight;
            scrollOffset -= mouseWheelMove * scrollSpeed;
        }
        float maxScroll = std::max(0.0f, (data.size() + 1) * rowHeight - bounds.height);
        scrollOffset = Clamp(scrollOffset, 0.0f, maxScroll);
    }

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
            float maxOffset = (totalLines - visibleLines) * (fontSize + lineSpacing);
            scrollOffset += deltaY * (maxOffset / (bounds.height - scrollBarHeight));
            scrollOffset = Clamp(scrollOffset, 0.0f, maxOffset);
        } else {
            isDraggingScrollBar = false;
        }
    }

    if (detailPopup) {
        if(!detailPopup->Update()) {
            delete detailPopup;
            detailPopup = nullptr;
        }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePosition, bounds_d) && !detailPopup) {
            int hoveredRow = (mousePosition.y - bounds_d.y + scrollOffset) / rowHeight;
            if (hoveredRow >= 0 && hoveredRow < (int)data.size()) {
                selectedRow = hoveredRow;
            }
        } else {
            selectedRow = -1; 
        }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && selectedRow != -1) {
        static double lastClickTime = 0.0;
        double currentTime = GetTime();
        if (currentTime - lastClickTime < 0.3) { 
            if (detailPopup == nullptr) {
                std::string connectionDetails = ConnectionInfoToString(data[selectedRow]);
                detailPopup = new Popup(bounds.x + bounds.width / 2 - 250, bounds.y + bounds.height / 2 - 150, 500, 300, "Connection Details", connectionDetails, font);
            }
        }
        lastClickTime = currentTime;
    }

    if (detailPopup && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!CheckCollisionPointRec(mousePosition, bounds) && !CheckCollisionPointRec(mousePosition, detailPopup->GetBounds())) {
            delete detailPopup;
            detailPopup = nullptr;
        }
    }
}

void Table::Draw() {
    DrawRectangleRounded(bounds, 0.01, 10, EMPTY_COLOR);
    DrawRectangleRoundedLinesEx(bounds, 0.01, 10, 2, DARKGRAY);

    float columnX[] = { bounds.x + 10, bounds.x + 120, bounds.x + 300, bounds.x + 500 };
    float headerY = bounds.y; 
    DrawRectangle(bounds.x, headerY, bounds.width, rowHeight, DARKGRAY); 
    DrawTextEx(font, "Method", { columnX[0], headerY + 5 }, fontSize, 1, WHITE);
    DrawTextEx(font, "IP Source", { columnX[1], headerY + 5 }, fontSize, 1, WHITE);
    DrawTextEx(font, "IP Remote", { columnX[2], headerY + 5 }, fontSize, 1, WHITE);
    DrawTextEx(font, "URL", { columnX[3], headerY + 5 }, fontSize, 1, WHITE);

    BeginScissorMode(bounds.x, bounds.y + rowHeight, bounds.width, bounds.height - rowHeight);

    float y = bounds.y + rowHeight - scrollOffset; 
    for (size_t i = 0; i < data.size(); ++i) {
        bool isHovered = CheckCollisionPointRec(GetMousePosition(), { bounds.x, y, bounds.width, rowHeight });
        DrawRow(i, y, isHovered, false);
        y += rowHeight;
    }

    EndScissorMode();

    float maxScroll = std::max(0.0f, (data.size() + 1) * rowHeight - bounds.height);
    if (maxScroll > 0) {
        float scrollBarHeight = (bounds.height - rowHeight) * (bounds.height - rowHeight) / (data.size() * rowHeight);
        float scrollBarY = bounds.y + rowHeight + ((bounds.height - rowHeight - scrollBarHeight) * (scrollOffset / maxScroll));
        DrawRectangle(bounds.x + bounds.width - 5, bounds.y + rowHeight, 5, bounds.height - rowHeight, DARKGRAY); 
        DrawRectangle(bounds.x + bounds.width - 5, scrollBarY, 5, scrollBarHeight, BLACK); 
    }

    if (detailPopup) detailPopup->Draw();
}


// --------------------------- InputText Class ---------------------------
InputText::InputText(float x, float y, float width, float height, int maxLength, 
                     Font customFont, int textSize, Color baseCol)
    : bounds{ x, y, width, height }, maxChars(maxLength), baseColor{ baseCol }, borderColor{ DARKGRAY },
      textColor{ BLACK }, cursorColor{ BLACK }, isActive(false), cursorPosition(0), blinkTime(0.0),
      textOffsetX(0), backspaceTimer(0.0), font(customFont), fontSize(textSize), text("") {}

void InputText::Update() {
    Vector2 mousePoint = GetMousePosition();
    if (CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isActive = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isActive = false;
    }

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

        if (IsKeyDown(KEY_BACKSPACE)) {
            backspaceTimer += GetFrameTime();
            if (cursorPosition > 0 && (backspaceTimer > 0.5 || IsKeyPressed(KEY_BACKSPACE))) {
                text.erase(cursorPosition - 1, 1);
                cursorPosition--;
                backspaceTimer = (backspaceTimer > 0.5) ? 0.05 : 0.0; 
                AdjustTextOffset();
            }
        } else {
            backspaceTimer = 0.0;
        }

        if (IsKeyPressed(KEY_LEFT) && cursorPosition > 0) {
            cursorPosition--;
            AdjustTextOffset();
        }
        if (IsKeyPressed(KEY_RIGHT) && cursorPosition < (int)text.length()) {
            cursorPosition++;
            AdjustTextOffset();
        }
    }

    blinkTime += GetFrameTime();
}

void InputText::AdjustTextOffset() {
    float cursorX = MeasureTextEx(font, text.substr(0, cursorPosition).c_str(), fontSize, 1).x;

    if (cursorX - textOffsetX > bounds.width - 20) {
        textOffsetX = cursorX - (bounds.width - 20);
    } else if (cursorX - textOffsetX < 0) {
        textOffsetX = cursorX;
    }

    if (textOffsetX < 0) textOffsetX = 0;
}

void InputText::Draw() {
    DrawRectangleRounded(bounds, 0.2, 10, baseColor);
    DrawRectangleRoundedLinesEx(bounds, 0.2, 10, 2, borderColor);

    Rectangle clip = { bounds.x + 10, bounds.y, bounds.width - 20, bounds.height };

    BeginScissorMode(clip.x, clip.y, clip.width, clip.height);

    float textX = bounds.x + 10 - textOffsetX;
    float textY = bounds.y + (bounds.height - fontSize) / 2;
    DrawTextEx(font, text.c_str(), { textX, textY }, fontSize, 1, textColor);

    EndScissorMode();

    if (isActive && ((int)(blinkTime * 2) % 2 == 0)) {
        float cursorX = textX + MeasureTextEx(font, text.substr(0, cursorPosition).c_str(), fontSize, 1).x;
        DrawLineV({ cursorX, textY }, { cursorX, textY + fontSize }, cursorColor);
    }
}

std::string InputText::GetText() const {
    return text;
}

void InputText::SetText(const std::string& newText) {
    text = newText;
    cursorPosition = text.length();
    textOffsetX = 0;
}


// --------------------------- InputFieldWithButton Class ---------------------------
InputFieldWithButton::InputFieldWithButton(float inputX, float inputY, float inputWidth, float inputHeight,
                                           const std::string& buttonText, float buttonX, float buttonY,
                                           float buttonWidth, float buttonHeight, Font font,
                                           Color buttonCol, Color hoverCol, Color inputCol)
    : inputField(inputX, inputY, inputWidth, inputHeight, 256, font, 20, inputCol),
      actionButton(buttonX, buttonY, buttonWidth, buttonHeight, buttonText, 20.f, 10.f, font, buttonCol, hoverCol) {}

bool InputFieldWithButton::Update() {
    inputField.Update();
    return actionButton.Update();
}

void InputFieldWithButton::Draw() {
    inputField.Draw();
    actionButton.Draw();
}

bool InputFieldWithButton::IsButtonClicked() const {
    return actionButton.IsClicked();
}

std::string InputFieldWithButton::GetInputText() const {
    return inputField.GetText();
}

void InputFieldWithButton::clear() {
    inputField.SetText("");
}

void InputFieldWithButton::SetText(const std::string& newText) {
    inputField.SetText(newText);
}


// --------------------------- NameList Class ---------------------------
NameList::NameList(std::string filename, float x, float y, float width, float height, 
                   std::unordered_set<std::string>& names, Font customFont, 
                   const std::string& titleText, int textSize, int rowSpacing)
    : fileName(filename), bounds{x, y, width, height - 50}, nameSet(names), font(customFont),
      inputFieldWithButton(x + 10, y + height - 40, width - 120, 30, "Add", x + width - 100, y + height - 40, 90, 30, customFont, NORMAL_BUTTON_COLOR, SECONDARY_HOVERED_BUTTON_COLOR),
      showContextMenu(false), contextMenuPosition{0, 0}, selectedNameIndex(-1), scrollOffset(0.0f), title(titleText),
      bounds_d{x, y + rowHeight, width, height - rowHeight - 50}, fontSize(textSize), lineSpacing(rowSpacing) 
    {
        UpdateNameVector();
        visibleRows = bounds.height / (fontSize + lineSpacing);
    }

void NameList::Update() {
    UpdateNameVector();
    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, bounds)) {
        float mouseWheelMove = GetMouseWheelMove();
        if (mouseWheelMove != 0.0f) {
            float scrollSpeed = rowHeight; 
            scrollOffset -= mouseWheelMove * scrollSpeed;
        }

        float maxScroll = std::max(0.0f, (nameVector.size() + 1) * rowHeight - bounds.height);
        scrollOffset = Clamp(scrollOffset, 0.0f, maxScroll);
    }

    HandleScrollBar(mousePoint);

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

    HandleContextMenu(mousePoint);
}

void NameList::HandleScrollBar(Vector2 mousePosition) {
    float scrollBarHeight = (bounds.height * visibleRows) / nameVector.size();
    Rectangle scrollBar = { bounds.x + bounds.width - 5,
        bounds.y + (bounds.height - scrollBarHeight) * (scrollOffset / ((nameVector.size() - visibleRows) * (fontSize + lineSpacing))), 
        5, 
        scrollBarHeight 
    };

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

            float maxOffset = (nameVector.size() - visibleRows) * (fontSize + lineSpacing);
            scrollOffset += deltaY * (maxOffset / (bounds.height - scrollBarHeight));
            scrollOffset = Clamp(scrollOffset, 0.0f, maxOffset);
        } else {
            isDraggingScrollBar = false;
        }
    }
}

void NameList::HandleContextMenu(Vector2 mousePoint) {
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

    if (showContextMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle deleteOption = {contextMenuPosition.x, contextMenuPosition.y, 80, 30};
        if (CheckCollisionPointRec(mousePoint, deleteOption)) {
            nameSet.erase(nameVector[selectedNameIndex]);
            SaveToFile();
            UpdateNameVector();
            showContextMenu = false;
        } else {
            showContextMenu = false;
        }
    }
}

void NameList::Draw() {
    DrawRectangleRounded(bounds, 0.01, 10, EMPTY_COLOR);
    DrawRectangleRoundedLinesEx(bounds, 0.01, 10, 2, DARKGRAY);

    DrawRectangle(bounds.x, bounds.y, bounds.width, rowHeight, TITLE_COLOR); 
    DrawTextEx(font, title.c_str(), {bounds.x + 10, bounds.y + 5}, 20, 1, BLACK);

    BeginScissorMode(bounds.x, bounds.y + rowHeight, bounds.width, bounds.height - rowHeight);

    float yPosition = bounds.y + rowHeight - scrollOffset;
    for (size_t index = 0; index < nameVector.size(); index++) {
        Rectangle nameBounds = {bounds.x, yPosition, bounds.width, rowHeight};
        Color hoverColor = CheckCollisionPointRec(GetMousePosition(), nameBounds) ? WHITE : FILLED_COLOR;

        DrawRectangleRec(nameBounds, hoverColor);
        DrawTextEx(font, nameVector[index].c_str(), {nameBounds.x + 10, yPosition + 5}, 20, 1, DATA_COLOR);
        yPosition += rowHeight;
    }

    EndScissorMode();

    float maxScroll = std::max(0.0f, (nameVector.size() + 1) * rowHeight - bounds.height);
    if (maxScroll > 0) {
        float scrollBarHeight = (bounds.height - rowHeight) * (bounds.height - rowHeight) / (nameVector.size() * rowHeight);
        float scrollBarY = bounds.y + rowHeight + ((bounds.height - rowHeight - scrollBarHeight) * (scrollOffset / maxScroll));
        DrawRectangle(bounds.x + bounds.width - 5, bounds.y + rowHeight, 5, bounds.height - rowHeight, DARKGRAY); // Nền thanh cuộn
        DrawRectangle(bounds.x + bounds.width - 5, scrollBarY, 5, scrollBarHeight, BLACK); // Thanh cuộn
    }

    inputFieldWithButton.Draw();

    if (showContextMenu) {
        DrawRectangleRec({contextMenuPosition.x, contextMenuPosition.y, 80, 30}, PRESS_COLOR);
        DrawTextEx(font, "Delete", {contextMenuPosition.x + 10, contextMenuPosition.y + 5}, 20, 1, WHITE);
    }
}

void NameList::SaveToFile() {
    std::ofstream outFile(fileName);
    for (const auto& name : nameSet) {
        outFile << name << "\n";
    }
    outFile.close();
}

void NameList::UpdateNameVector() {
    nameVector = std::vector<std::string>(nameSet.begin(), nameSet.end());
    std::sort(nameVector.begin(), nameVector.end());
}


