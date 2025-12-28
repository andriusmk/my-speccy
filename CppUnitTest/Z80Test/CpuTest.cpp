//      Copyright © 2025  Andrius Mazeikis
//
//      This program is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "Z80/Cpu.hpp"
#include "Mocks/BusMock.hpp"
#include "Mocks/PrimitivesMock.hpp"
#include "Mocks/RegistersMock.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>

using namespace Z80;
using ::testing::_;

bool operator==(const CpuState& st1, const CpuState& st2)
{
    auto first = reinterpret_cast<const uint8_t*>(&st1);
    auto second = reinterpret_cast<const uint8_t*>(&st2);

    return std::equal(first, first + sizeof(CpuState), second);
}

class CpuTest : public ::testing::Test
{
    void SetUp() final override
    {
        initialState = state;
        cpu = std::make_unique<Cpu>(memory, io, &state);
    }

    void TearDown() final override
    {
        state.R = initialState.R;
        state.PC = initialState.PC;

        EXPECT_TRUE(state == initialState);
    }

  protected:
    BusMock memory;
    BusMock io;
    CpuState state;
    CpuState initialState;
    std::unique_ptr<Cpu> cpu;
};

TEST(CpuStandalone, DISABLED_ConstructionDestructionNoSideEffects)
{
    BusMock memory;
    BusMock io;

    EXPECT_CALL(memory, read(_)).Times(0);
    EXPECT_CALL(memory, write(_, _)).Times(0);
    EXPECT_CALL(io, read(_)).Times(0);
    EXPECT_CALL(io, write(_, _)).Times(0);

    Cpu cpu(memory, io);
}

TEST_F(CpuTest, DISABLED_ResetNoSideEffects)
{
    EXPECT_CALL(memory, read(_)).Times(0);
    EXPECT_CALL(memory, write(_, _)).Times(0);
    EXPECT_CALL(io, read(_)).Times(0);
    EXPECT_CALL(io, write(_, _)).Times(0);

    cpu->reset();
}
