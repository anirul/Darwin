#pragma once

#include <string>
#include <memory>

#include "darwin_client.h"
#include "world_client.h"

namespace darwin {

    class NetworkApp {
    public:
        NetworkApp();
        void EnterWorld(const std::string& name);
        std::string GetName() const;
        bool IsConnected() const;
        void Run();
        bool Ping(std::int32_t& val);

    private:
        std::unique_ptr<darwin::DarwinClient> darwin_client_;
        darwin::WorldClient world_client_;
        std::string name_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
    };

}  // namespace darwin.