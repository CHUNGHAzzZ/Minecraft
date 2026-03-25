#include "Input.h"

namespace Minecraft {

std::unordered_set<Qt::Key> Input::s_KeysPressed;
std::unordered_set<Qt::Key> Input::s_KeysJustPressed;
std::unordered_set<Qt::Key> Input::s_KeysJustReleased;
std::unordered_set<Qt::MouseButton> Input::s_MouseButtonsPressed;
glm::vec2 Input::s_MousePosition = glm::vec2(0.0f);
glm::vec2 Input::s_LastMousePosition = glm::vec2(0.0f);
glm::vec2 Input::s_MouseDelta = glm::vec2(0.0f);
float Input::s_ScrollDelta = 0.0f;

void Input::Init() {
    s_KeysPressed.clear();
    s_KeysJustPressed.clear();
    s_KeysJustReleased.clear();
    s_MouseButtonsPressed.clear();
}

void Input::Update() {
    s_KeysJustPressed.clear();
    s_KeysJustReleased.clear();
    s_MouseDelta = s_MousePosition - s_LastMousePosition;
    s_LastMousePosition = s_MousePosition;
    s_ScrollDelta = 0.0f;
}

bool Input::IsKeyPressed(Qt::Key key) {
    return s_KeysPressed.find(key) != s_KeysPressed.end();
}

bool Input::IsKeyJustPressed(Qt::Key key) {
    return s_KeysJustPressed.find(key) != s_KeysJustPressed.end();
}

bool Input::IsKeyJustReleased(Qt::Key key) {
    return s_KeysJustReleased.find(key) != s_KeysJustReleased.end();
}

bool Input::IsMouseButtonPressed(Qt::MouseButton button) {
    return s_MouseButtonsPressed.find(button) != s_MouseButtonsPressed.end();
}

void Input::OnKeyPress(Qt::Key key) {
    if (s_KeysPressed.find(key) == s_KeysPressed.end()) {
        s_KeysJustPressed.insert(key);
    }
    s_KeysPressed.insert(key);
}

void Input::OnKeyRelease(Qt::Key key) {
    s_KeysPressed.erase(key);
    s_KeysJustReleased.insert(key);
}

void Input::OnMousePress(Qt::MouseButton button) {
    s_MouseButtonsPressed.insert(button);
}

void Input::OnMouseRelease(Qt::MouseButton button) {
    s_MouseButtonsPressed.erase(button);
}

void Input::OnMouseMove(float x, float y) {
    s_MousePosition = glm::vec2(x, y);
}

void Input::OnMouseScroll(float delta) {
    s_ScrollDelta = delta;
}

} // namespace Minecraft
