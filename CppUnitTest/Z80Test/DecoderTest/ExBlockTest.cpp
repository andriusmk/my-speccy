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
#include "DecoderTest.hpp"

namespace Z80 {

TEST_F(DecoderTest, ExDEHL)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xEB));
    
    EXPECT_CALL(prim, ex(Reg16::DE, Reg16::HL));
    
    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_F(DecoderTest, ExAF)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x08));
    
    EXPECT_CALL(prim, ex(Reg16::AF, Reg16::AF_));
    
    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_F(DecoderTest, Exx)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xD9));
    
    EXPECT_CALL(prim, ex(Reg16::BC, Reg16::BC_));
    EXPECT_CALL(prim, ex(Reg16::DE, Reg16::DE_));
    EXPECT_CALL(prim, ex(Reg16::HL, Reg16::HL_));

    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_P(Values16Test, ExSP)
{
    auto value = GetParam();
    
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xE3));
    
    EXPECT_CALL(regs, get(Reg16::SP)).WillOnce(Return(value));
    EXPECT_CALL(prim, ex(value, Reg16::HL));
    
    EXPECT_EQ(decoder.decodeOne(), 19);
}

TEST_P(Values16IdxTest, ExSP)
{
    auto [value, idx] = GetParam();
    auto [reg, prefix] = idx;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0xE3));
    
    EXPECT_CALL(regs, get(Reg16::SP)).WillOnce(Return(value));
    EXPECT_CALL(prim, ex(value, reg));
    
    EXPECT_EQ(decoder.decodeOne(), 23);
}

}
