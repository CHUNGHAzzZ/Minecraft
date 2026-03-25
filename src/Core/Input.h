#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include <unordered_set>

namespace Minecraft {

class Input {
public:
    static void Init();
    static void Update();
    
    // Keyboard
    static bool IsKeyPressed(Qt::Key key);
    static bool IsKeyJustPressed(Qt::Key key);
    static bool IsKeyJustReleased(Qt::Key key);
    
    // Mouse
    static bool IsMouseButtonPressed(Qt::MouseButton button);
    static glm::vec2 GetMousePosition() { return s_MousePosition; }
    static glm::vec2 GetMouseDelta() { return s_MouseDelta; }
    static float GetMouseScrollDelta() { return s_ScrollDelta; }
    
    // Internal update methods (called by Window)
    static void OnKeyPress(Qt::Key key);
    static void OnKeyRelease(Qt::Key key);
    static void OnMousePress(Qt::MouseButton button);
    static void OnMouseRelease(Qt::MouseButton button);
    static void OnMouseMove(float x, float y);
    static void OnMouseScroll(float delta);

private:
    static std::unordered_set<Qt::Key> s_KeysPressed;
    static std::unordered_set<Qt::Key> s_KeysJustPressed;
    static std::unordered_set<Qt::Key> s_KeysJustReleased;
    
    static std::unordered_set<Qt::MouseButton> s_MouseButtonsPressed;
    static glm::vec2 s_MousePosition;
    static glm::vec2 s_LastMousePosition;
    static glm::vec2 s_MouseDelta;
    static float s_ScrollDelta;
};

} // namespace Minecraft
