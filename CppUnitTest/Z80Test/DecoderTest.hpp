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
#pragma once

#include "Z80/Cpu/Decoder.hpp"
#include "Mocks/PrimitivesMock.hpp"
#include "Mocks/BusMock.hpp"
#include "Mocks/RegistersMock.hpp"

#include <gtest/gtest.h>
#include <cstdint>

namespace Z80 {

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

class DecoderTest : public ::testing::Test
{
public:
    void SetUp() final override
    {
        
    }
    
    void TearDown() final override
    {
        
    }
    
    template<typename... Args>
    void setInitialFlags(Args... args)
    {
        flags = Flags::combine(args...);
        EXPECT_CALL(regs, get(Reg8::Flags)).WillOnce(Return(flags));
        EXPECT_CALL(regs, set(Reg8::Flags, _)).WillOnce(SaveArg<1>(&flags));
    }
    
    template<typename... Args>
    void verifyFlags(Args... args)
    {
        EXPECT_EQ(flags, Flags::combine(args...));
    }

    BusMock mem;
    BusMock io;
    PrimitivesMock prim;
    RegistersMock regs;
    Parts parts {.mem = mem, .io = io, .regs = regs, .prim = prim, .dec = decoder };
    Decoder decoder{parts};
    uint8_t flags;
};

}
