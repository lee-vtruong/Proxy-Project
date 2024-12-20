#ifndef GUI_H
#define GUI_H

#include "http_parser.h"
#include "common_lib.h"

class Button {
protected:
    Rectangle bounds;           // 
    Color baseColor;            // 
    Color hoverColor;           // 
    Color pressColor;           // 
    Color textColor;            // 
    Color hoverTextColor;       //
    std::string text;           // 
    int fontSize;               // 
    Font font;                  // 
    bool isHovered;             //
    bool isPressed;             // 
    float cornerRadius;         // 

public:
    Button(float x, float y, float width, float height, const std::string& buttonText, int textSize = 20, 
           float radius = 10.0f, Font customFont = GetFontDefault(), Color base = GRAY, Color hover = LIGHTGRAY,
           Color press = DARKGRAY, Color textCol = WHITE, Color hoverText = WHITE);

    bool Update();
    void Draw();
    bool IsClicked() const;
    void SetText(const std::string& newText);
    void SetCornerRadius(float radius);
    void SetFont(Font customFont);
};


class ToggleButton : public Button {
private:
    std::string labelOn;        // 
    std::string labelOff;       // 
    bool isOn;                  // 

public:
    ToggleButton(float x, float y, float width, float height, 
                 const std::string& textOn, const std::string& textOff, int textSize = 20, 
                 float radius = 10.0f, Font customFont = GetFontDefault(), Color base = GRAY, 
                 Color hover = LIGHTGRAY, Color press = DARKGRAY, Color textCol = WHITE, Color hoverText = WHITE);

    int Update();
    bool GetState() const;
};


class TextBox {
private:
    Rectangle bounds;                       // 
    Color borderColor;                      // 
    Color backgroundColor;                  // 
    Color textColor;                        // 
    Font font;                              // 
    int fontSize;                           // 
    float cornerRadius;                     // 
    std::string text;                       // 
    float scrollOffset;                     // 
    float lineSpacing;                      //
    int visibleLines;                       // 
    int totalLines;                         //
    std::vector<std::string> wrappedText;   //
    bool isDraggingScrollBar;               // 
    float dragStartY;                       // 

    void WrapText();

public:
;    TextBox(float x, float y, float width, float height, const std::string& content,
            Font customFont = GetFontDefault(), int textSize = 20, float radius = 10.0f,
            Color bg = CONTENT_BOX_COLOR, Color border = GRAY, Color textCol = BLACK, float spacing = 5.0f);

    void Update();
    void Draw();
    void SetText(const std::string& newText);
    std::string GetText() const;
};


class Popup {
private:
    Rectangle bounds;               // 
    Color backgroundColor;          // 
    Color borderColor;              //
    Color titleBarColor;            // 
    Color textColor;                // 
    std::string title;              // 
    TextBox contentBox;             // 
    float cornerRadius;             // 
    bool isDragging;                // 
    Vector2 dragOffset;             // 
    bool isVisible;                 // 
    Font font;
public:
    Popup(float x, float y, float width, float height, const std::string& titleText, const std::string& content,
          Font customFont = GetFontDefault(), int textSize = 20, float radius = 10.0f,
          Color bg = LIGHTGRAY, Color border = GRAY, Color titleColor = DARKGRAY, Color textCol = BLACK);

    bool Update();
    void Draw();
    void SetVisible(bool visible);
    bool IsVisible() const;
    void SetContent(const std::string& content);
    Rectangle GetBounds() const;
};


class Table {
private:
    Rectangle bounds;                       // 
    std::vector<ConnectionInfo> data;       //
    Font font;                              // 
    int fontSize;                           // 
    float rowHeight;                        // 
    float scrollOffset;                     // 
    int selectedRow;                        // 
    float lineSpacing;                      // 
    bool isDraggingScrollBar;               // 
    float dragStartY;                       // 
    Popup* detailPopup;                     // 
    int visibleLines;                       // 
    int totalLines;                         //
    Rectangle bounds_d;

    void DrawRow(int rowIndex, float y, bool isHovered, bool isSelected);
    void WrapText();
public:
    Table(float x, float y, float width, float height, const std::vector<ConnectionInfo>& connectionData,
          Font customFont = GetFontDefault(), int textSize = 20, float rowSpacing = 5.0f);

    void Update(std::vector<ConnectionInfo>& connections);
    void Draw();
};


class InputText {
private:
    Rectangle bounds;           //
    int maxChars;               //
    Color baseColor;            // 
    Color borderColor;          // 
    Color textColor;            // 
    Color cursorColor;          // 
    bool isActive;              // 
    int cursorPosition;         // 
    double blinkTime;           // 
    float textOffsetX;          // 
    double backspaceTimer;      // 
    Font font;                  //
    int fontSize;               // 
    std::string text;           //

public:
    InputText(float x, float y, float width, float height, int maxLength = 64,
              Font customFont = GetFontDefault(), int textSize = 20, Color baseCol = LIGHTGRAY);

    void Update();
    void AdjustTextOffset();
    void Draw();
    std::string GetText() const;
    void SetText(const std::string& newText);
};


class InputFieldWithButton {
private:
    InputText inputField;
    Button actionButton;

public:
    InputFieldWithButton(float inputX, float inputY, float inputWidth, float inputHeight,
                          const std::string& buttonText, float buttonX, float buttonY, float buttonWidth,
                          float buttonHeight, Font font = GetFontDefault(), Color buttonCol = NORMAL_BUTTON_COLOR,
                          Color hoverCol = SECONDARY_HOVERED_BUTTON_COLOR, Color inputCol = WHITE);

    bool Update();
    void Draw();
    bool IsButtonClicked() const;
    std::string GetInputText() const;
    void clear();
    void SetText(const std::string& newText);
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
    float scrollOffset; 
    std::string title;
    std::vector<std::string> nameVector;
    const float rowHeight = 30; 
    int visibleRows; 
    Rectangle bounds_d;
    int fontSize;
    int lineSpacing;
public:
    NameList(std::string filename, float x, float y, float width, float height, 
             std::unordered_set<std::string>& names, Font customFont,
             const std::string& titleText, int textSize = 20, int rowSpacing = 5.0f);  
    
    void Update();
    void HandleScrollBar(Vector2 mousePosition);
    void HandleContextMenu(Vector2 mousePoint);
    void Draw();
    void SaveToFile();
    void UpdateNameVector();
};

#endif // GUI_H