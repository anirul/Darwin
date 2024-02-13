#include <iostream>
#include <utility>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "frame/common/application.h"
#include "frame/file/file_system.h"
#include "frame/file/image_stb.h"
#include "frame/window_factory.h"
#include "frame/gui/draw_gui_factory.h"
#include "frame/gui/window_logger.h"
#include "frame/logger.h"
#include "world_client.h"

#if defined(_WIN32) || defined(_WIN64)
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd) try
{
#else
int main(int ac, char** av) try 
{
#endif
    auto win = frame::CreateNewWindow(
        frame::DrawingTargetEnum::WINDOW,
        frame::RenderingAPIEnum::OPENGL,
        { 1280, 720 });
    auto gui_window = frame::gui::CreateDrawGui(*win.get(), {}, 20.0f);
    gui_window->AddWindow(std::make_unique<frame::gui::WindowLogger>("Logger"));
    win->GetDevice().AddPlugin(std::move(gui_window));
    frame::common::Application app(std::move(win));
    app.Startup(frame::file::FindFile("asset/json/darwin_client.json"));
    frame::Logger& logger = frame::Logger::GetInstance();
    int i = 0;
    // Add a load from file for resolution.
    app.Run([&logger, &i] {
            if (i == 0) logger->warn("Hello");
            if (i == 1) logger->error("World");
            i++;
        });
    return 0;
}
catch (std::exception ex) {
#if defined(_WIN32) || defined(_WIN64)
    MessageBox(nullptr, ex.what(), "Exception", MB_ICONEXCLAMATION);
#else
    std::cerr << "Error: " << ex.what() << std::endl;
#endif
    return -2;
}
