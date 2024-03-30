#include "input_acquisition.h"

#include <SDL2/SDL.h>

namespace darwin {

    bool InputAcquisition::KeyPressed(char key, double dt) {
        if (!keys_.contains(key)) {
            keys_.insert(key);
        }   
        return true;
    }

    bool InputAcquisition::KeyReleased(char key, double dt) {
        if (keys_.contains(key)) {
            keys_.erase(key);
        }
        return true;
    }

    bool InputAcquisition::MouseMoved(
        glm::vec2 position,
        glm::vec2 relative,
        double dt) 
    {
        mouse_position_ += relative;
        return true;
    }

    bool InputAcquisition::MousePressed(char button, double dt) {
        if (SDL_BUTTON(SDL_BUTTON_LEFT)) {
            mouse_buttons_.insert(SDL_BUTTON_LEFT);
        }
        if (SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
            mouse_buttons_.insert(SDL_BUTTON_MIDDLE);
        }
        if (SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            mouse_buttons_.insert(SDL_BUTTON_RIGHT);
        }
        return true;
    }

    bool InputAcquisition::MouseReleased(char button, double dt) {
        if (SDL_BUTTON(SDL_BUTTON_LEFT)) {
            mouse_buttons_.erase(SDL_BUTTON_LEFT);
        }
        if (SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
            mouse_buttons_.erase(SDL_BUTTON_MIDDLE);
        }
        if (SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            mouse_buttons_.erase(SDL_BUTTON_RIGHT);
        }
        return true;
    }

    bool InputAcquisition::WheelMoved(float relative, double dt) {
        mouse_wheel_ += relative;
        return true;
    }

    void InputAcquisition::NextFrame() {
    }

    glm::vec2 InputAcquisition::GetMousePosition() {
        return mouse_position_;
    }
  
    bool InputAcquisition::IsJumping() const {
        return keys_.contains(' ');
    }

    float InputAcquisition::GetMouseWheel() {
        return mouse_wheel_;
    }

    bool InputAcquisition::IsMoving() const {
        return (keys_.contains('w') ^ keys_.contains('s')) ||
               (keys_.contains('a') ^ keys_.contains('d'));
    }

    bool InputAcquisition::IsMouseLeft() const {
        return mouse_buttons_.contains(SDL_BUTTON_LEFT);
    }

    bool InputAcquisition::IsMouseRight() const {
        return mouse_buttons_.contains(SDL_BUTTON_RIGHT);
    }

    bool InputAcquisition::IsMouseMiddle() const {
        return mouse_buttons_.contains(SDL_BUTTON_MIDDLE);
    }

    float InputAcquisition::GetHorizontal() const {
        float horizontal = 0.0f;
        if (keys_.contains('a')) {
            horizontal -= 1.0f;
        }
        if (keys_.contains('d')) {
            horizontal += 1.0f;
        }
        return horizontal;
    }

    float InputAcquisition::GetVertical() const {
        float vertical = 0.0f;
        if (keys_.contains('w')) {
            vertical += 1.0f;
        }
        if (keys_.contains('s')) {
            vertical -= 1.0f;
        }
        return vertical;
    }

} // namespace darwin.
