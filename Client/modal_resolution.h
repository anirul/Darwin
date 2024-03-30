#include "state_resolution.h"

namespace darwin::modal {

    enum class ModalResolutionButton {
        None,
        Change,
        Start,
    };

    struct ModalResolutionParams {
        glm::uvec2 resolution = { 1280, 720 };
        frame::FullScreenEnum fullscreen_enum;
        ModalResolutionButton button_result = ModalResolutionButton::None;
    };
    
    class ModalResolution : public frame::gui::GuiWindowInterface {
    public:
        ModalResolution(
            const std::string& name, 
            ModalResolutionParams& params);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalResolutionParams& params_;
        bool end_ = false;
    };

} // namespace darwin::modal.
