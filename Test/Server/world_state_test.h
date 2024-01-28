#pragma once

#include "Server/world_state.h"
#include <gtest/gtest.h>

namespace test {

    class WorldStateTest : public testing::Test {
    public:
        WorldStateTest() = default;

    protected:
        std::unique_ptr<darwin::WorldState> world_state_;
    };

} // namespace test.
