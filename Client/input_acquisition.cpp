#include "input_acquisition.h"

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
        double dt) {
        mouse_position_ = position;
        return true;
    }

    bool InputAcquisition::MousePressed(char button, double dt) {
        return true;
    }

    bool InputAcquisition::MouseReleased(char button, double dt) {
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
        return 
            keys_.contains('w') || 
            keys_.contains('a') || 
            keys_.contains('s') || 
            keys_.contains('d');
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
