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

#include "Mocks/RegistersMock.hpp"

#include <gtest/gtest.h>

namespace Z80 {

using ::testing::Return;

class FlagsTest : public ::testing::Test
{
public:
    void SetUp()
    {
    }
    
    void TearDown()
    {
    }

    RegistersMock regs;
};

TEST_F(FlagsTest, NoChange)
{
    EXPECT_CALL(regs, get(Reg8::Flags)).WillOnce(Return(0x42));
    EXPECT_CALL(regs, set(Reg8::Flags, 0x42));
    
    Flags flags(regs);
}

TEST_F(FlagsTest, SetCF)
{
    EXPECT_CALL(regs, get(Reg8::Flags)).WillOnce(Return(0));
    EXPECT_CALL(regs, set(Reg8::Flags, 0x01));
    
    Flags flags(regs);
    
    flags.setFrom(0xFF, Flag::C);
}

TEST_F(FlagsTest, ClearZF)
{
    EXPECT_CALL(regs, get(Reg8::Flags)).WillOnce(Return(0xFF));
    EXPECT_CALL(regs, set(Reg8::Flags, 0xBF));
    
    Flags flags(regs);
    
    flags.setFrom(0x00, Flag::Z);
}

TEST_F(FlagsTest, SetHF)
{
    EXPECT_CALL(regs, get(Reg8::Flags)).WillOnce(Return(0));
    EXPECT_CALL(regs, set(Reg8::Flags, 0x10));
    
    Flags flags(regs);
    
    flags.set(Flag::H);
}

TEST_F(FlagsTest, ClearNF)
{
    EXPECT_CALL(regs, get(Reg8::Flags)).WillOnce(Return(0xFF));
    EXPECT_CALL(regs, set(Reg8::Flags, 0xFD));
    
    Flags flags(regs);
    
    flags.clear(Flag::N);
}

}
