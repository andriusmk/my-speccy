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
#include <tuple>
#include <array>

namespace Z80 {

using ::testing::ValuesIn;
using ::testing::Combine;
using ::testing::Return;

using DDReg = std::tuple<Reg16, uint8_t>;
using DDRegTestParam = std::tuple<DDReg, uint16_t>;

constexpr std::array<DDReg, 4> ddRegs {
    std::make_tuple(Reg16::BC, 0b00000000),
    std::make_tuple(Reg16::DE, 0b00010000),
    std::make_tuple(Reg16::HL, 0b00100000),
    std::make_tuple(Reg16::SP, 0b00110000)
};

constexpr std::array<DDReg, 2> idxRegs {
    std::make_tuple(Reg16::IX, 0xDD),
    std::make_tuple(Reg16::IY, 0xFD)
};

constexpr std::array<uint16_t, 10> values16 {
    0x0000,
    0x5555,
    0x5500,
    0x0055,
    0xAAAA,
    0xAA00,
    0x00AA,
    0x55AA,
    0xAA55,
    0xFFFF
};


class DDRegisterTest : public DecoderTest, public ::testing::WithParamInterface<DDRegTestParam>
{
};

INSTANTIATE_TEST_SUITE_P(, DDRegisterTest, Combine(ValuesIn(ddRegs), ValuesIn(values16)));

class IdxRegTest : public DecoderTest, public ::testing::WithParamInterface<DDRegTestParam>
{
};

INSTANTIATE_TEST_SUITE_P(, IdxRegTest, Combine(ValuesIn(idxRegs), ValuesIn(values16)));

TEST_P(DDRegisterTest, LoadImmediate)
{
    const auto [regWithId, value] = GetParam();
    const auto [reg, regId] = regWithId;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(0x01 | regId));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(value));
    EXPECT_CALL(regs, set(reg, value));
    
    EXPECT_EQ(decoder.decodeOne(), 10);
}

TEST_P(IdxRegTest, LoadImmediate)
{
    const auto [regWithPrefix, value] = GetParam();
    const auto [reg, prefix] = regWithPrefix;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0x21));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(value));
    EXPECT_CALL(regs, set(reg, value));
    
    EXPECT_EQ(decoder.decodeOne(), 14);
}

}
