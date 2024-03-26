#pragma once

#include <set>

#include "frame/input_interface.h"

namespace darwin {

    class InputAcquisition : public frame::InputInterface {
    public:
        bool KeyPressed(char key, double dt) override;
        bool KeyReleased(char key, double dt) override;
        bool MouseMoved(
            glm::vec2 position, 
            glm::vec2 relative, 
            double dt) override;
        bool MousePressed(char button, double dt) override;
        bool MouseReleased(char button, double dt) override;
        bool WheelMoved(float relative, double dt) override;
        void NextFrame() override;

    public:
        glm::vec2 GetMousePosition();
        float GetMouseWheel();
        bool IsJumping() const;
        bool IsMoving() const;
        bool IsTab() const;
        float GetHorizontal() const;
        float GetVertical() const;

    private:
        glm::vec2 mouse_position_;
        float mouse_wheel_;
        std::set<char> keys_;
    };

} // namespace darwin.
