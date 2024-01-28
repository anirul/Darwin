#pragma once

#include "Server/world_state_file.h"
#include <gtest/gtest.h>

namespace test {

    class WorldStateFileTest : public testing::Test {
    public:
        WorldStateFileTest() = default;
        void PopulateWorldState();

    protected:
        darwin::WorldState world_state_;
    };

} // namespace test.