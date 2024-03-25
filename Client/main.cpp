#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <grpc/grpc.h>
#include <grpc/support/log.h>

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "state_context.h"
#include "state_title.h"
#include "frame/common/application.h"
#include "frame/file/file_system.h"
#include "frame/file/image_stb.h"
#include "frame/window_factory.h"
#include "frame/gui/draw_gui_factory.h"
#include "frame/gui/window_logger.h"
#include "frame/logger.h"
#include "frame/json/parse_level.h"
#include "modal_stats.h"
#include "Common/stl_proto_wrapper.h"
#include "overlay_font.h"
#include "Common/client_audio.pb.h"
#include "audio_system.h"

void grpc_log_handler(gpr_log_func_args* args) {
    frame::Logger& logger = frame::Logger::GetInstance();
    switch (args->severity) {
    case GPR_LOG_SEVERITY_DEBUG:
        logger->debug("GRPC: {}", args->message);
        break;
    case GPR_LOG_SEVERITY_INFO:
        logger->info("GRPC: {}", args->message);
        break;
    case GPR_LOG_SEVERITY_ERROR:
        logger->error("GRPC: {}", args->message);
        break;
    }
}

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

    // Set grpc log handler.
    gpr_set_log_function(grpc_log_handler);
    // Set grpc log verbosity set it to DEBUG if you want to see more or
    // even INFO if you want to see all.
    gpr_set_log_verbosity(GPR_LOG_SEVERITY_ERROR);
    grpc_tracer_set_enabled("all", true);

    auto win = frame::CreateNewWindow(
        frame::DrawingTargetEnum::WINDOW,
        frame::RenderingAPIEnum::OPENGL,
        { 1280, 720 });
    // Load the client parameter.
    proto::ClientParameter client_parameter =
        darwin::LoadProtoFromJsonFile<proto::ClientParameter>(
            frame::file::FindFile("asset/json/client_parameter.json"));
    proto::ClientAudio client_audio =
        darwin::LoadProtoFromJsonFile<proto::ClientAudio>(
            frame::file::FindFile("asset/json/client_audio.json"));
    auto gui_window = frame::gui::CreateDrawGui(
        *win.get(),
        frame::file::FindFile("asset/font/axaxax/axaxax_bd.otf"),
        24.0f);
    gui_window->AddWindow(
        std::make_unique<frame::gui::WindowLogger>("Logger"));
    darwin::modal::ModalStatsParams params;
    gui_window->AddWindow(
        std::make_unique<darwin::modal::ModalStats>("Stats", params));
    // Start with the debug window hidden.
    gui_window->SetVisible(false);
    // Set the keyboard to be passed to the modal window.
    gui_window->SetKeyboardPassed(true);
    // Darkening the background of the modal window.
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(
        0.0f, 0.0f, 0.0f, 0.5f);
    auto* gui_window_ptr = gui_window.get();
    // Add a debugging key if you press on '`' key.
    win->AddKeyCallback(SDLK_F1, [gui_window_ptr] {
            if (gui_window_ptr == nullptr) {
                return false;
            }
            gui_window_ptr->SetVisible(!gui_window_ptr->IsVisible());
            return true;
        });
    // Add an exit key if you press on 'ESC' key.
    win->AddKeyCallback(27, [] {
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
            return true;
        });
    // Add plugin to the device.
    win->GetDevice().AddPlugin(std::move(gui_window));
    // Move the window to the application.
    frame::common::Application app(std::move(win));
    app.Startup(frame::file::FindFile("asset/json/darwin_client.json"));
    frame::Logger& logger = frame::Logger::GetInstance();
    int i = 0;
    darwin::audio::AudioSystem audio_system(client_audio);
    darwin::state::StateContext state_context(
        std::make_unique<darwin::state::StateTitle>(app, audio_system),
        client_parameter);
    // Add a load from file for resolution.
    app.Run([&state_context, &app] {
            state_context.Update();
        });
    return 0;
}
catch (std::exception ex)
{
#if defined(_WIN32) || defined(_WIN64)
    MessageBox(nullptr, ex.what(), "Exception", MB_ICONEXCLAMATION);
#else
    std::cerr << "Error: " << ex.what() << std::endl;
#endif
    return -2;
}
