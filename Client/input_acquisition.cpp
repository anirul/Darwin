#include "input_acquisition.h"

namespace darwin {

    bool InputAcquisition::KeyPressed(char key, double dt) {
        return true;
    }

    bool InputAcquisition::KeyReleased(char key, double dt) {
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

    float InputAcquisition::GetMouseWheel() {
        return mouse_wheel_;
    }

} // namespace darwin.
