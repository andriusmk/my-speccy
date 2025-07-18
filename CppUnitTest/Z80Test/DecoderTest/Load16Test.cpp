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
using TwoValuesParam = std::tuple<uint16_t, uint16_t>;
using TwoValuesIdxParam = std::tuple<uint16_t, uint16_t, DDReg>;

constexpr std::array<DDReg, 4> ddRegs {
    std::make_tuple(Reg16::BC, 0b00000000),
    std::make_tuple(Reg16::DE, 0b00010000),
    std::make_tuple(Reg16::HL, 0b00100000),
    std::make_tuple(Reg16::SP, 0b00110000)
};

constexpr std::array<DDReg, 4> qqRegs {
    std::make_tuple(Reg16::BC, 0b00000000),
    std::make_tuple(Reg16::DE, 0b00010000),
    std::make_tuple(Reg16::HL, 0b00100000),
    std::make_tuple(Reg16::AF, 0b00110000)
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

class TwoValues16Test : public DecoderTest, public ::testing::WithParamInterface<TwoValuesParam>
{
};

INSTANTIATE_TEST_SUITE_P(, TwoValues16Test, Combine(ValuesIn(values16), ValuesIn(values16)));

class TwoValues16IdxTest : public DecoderTest, public ::testing::WithParamInterface<TwoValuesIdxParam>
{
};

INSTANTIATE_TEST_SUITE_P(, TwoValues16IdxTest, Combine(ValuesIn(values16), ValuesIn(values16), ValuesIn(idxRegs)));

class TwoValues16RegTest : public DecoderTest, public ::testing::WithParamInterface<TwoValuesIdxParam>
{
};

INSTANTIATE_TEST_SUITE_P(, TwoValues16RegTest, Combine(ValuesIn(values16), ValuesIn(values16), ValuesIn(ddRegs)));

class Values16Test : public DecoderTest, public ::testing::WithParamInterface<uint16_t>
{
};

INSTANTIATE_TEST_SUITE_P(, Values16Test, ValuesIn(values16));

class Values16IdxTest : public DecoderTest,
public ::testing::WithParamInterface<std::tuple<uint16_t, DDReg>>
{
};

INSTANTIATE_TEST_SUITE_P(, Values16IdxTest, Combine(ValuesIn(values16), ValuesIn(idxRegs)));

class QQRegTest : public DecoderTest, public ::testing::WithParamInterface<DDReg>
{
};

INSTANTIATE_TEST_SUITE_P(, QQRegTest, ValuesIn(qqRegs));

class QQIdxRegTest : public DecoderTest, public ::testing::WithParamInterface<DDReg>
{
};

INSTANTIATE_TEST_SUITE_P(, QQIdxRegTest, ValuesIn(idxRegs));

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

TEST_P(TwoValues16Test, IndirectNNfromHL)
{
    auto [addr, value] = GetParam();
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x22));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(addr));
    EXPECT_CALL(regs, get(Reg16::HL)).WillOnce(Return(value));
    EXPECT_CALL(prim, write16(addr, value));
    
    EXPECT_EQ(decoder.decodeOne(), 16);
}

TEST_P(TwoValues16IdxTest, IndirectNNfromHL)
{
    auto [addr, value, idx] = GetParam();
    auto [reg, prefix] = idx;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0x22));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(addr));
    EXPECT_CALL(regs, get(reg)).WillOnce(Return(value));
    EXPECT_CALL(prim, write16(addr, value));
    
    EXPECT_EQ(decoder.decodeOne(), 20);
}

TEST_P(TwoValues16Test, IndirectNNtoHL)
{
    auto [addr, value] = GetParam();
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x2A));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(addr));
    EXPECT_CALL(prim, read16(addr)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(Reg16::HL, value));

    EXPECT_EQ(decoder.decodeOne(), 16);
}

TEST_P(TwoValues16IdxTest, IndirectNNtoHL)
{
    auto [addr, value, idx] = GetParam();
    auto [reg, prefix] = idx;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0x2A));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(addr));
    EXPECT_CALL(prim, read16(addr)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(reg, value));

    EXPECT_EQ(decoder.decodeOne(), 20);
}

TEST_P(TwoValues16RegTest, IndirectNNfromDDReg)
{
    auto [addr, value, ddreg] = GetParam();
    auto [reg, idx] = ddreg;

    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(0xED))
        .WillOnce(Return(0x43 | idx));

    EXPECT_CALL(prim, fetch16()).WillOnce(Return(addr));
    EXPECT_CALL(regs, get(reg)).WillOnce(Return(value));
    EXPECT_CALL(prim, write16(addr, value));

    EXPECT_EQ(decoder.decodeOne(), 20);
}

TEST_P(TwoValues16RegTest, IndirectNNtoDDReg)
{
    auto [addr, value, ddreg] = GetParam();
    auto [reg, idx] = ddreg;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(0xED))
        .WillOnce(Return(0x4B | idx));
    
    EXPECT_CALL(prim, fetch16()).WillOnce(Return(addr));
    EXPECT_CALL(prim, read16(addr)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(reg, value));

    EXPECT_EQ(decoder.decodeOne(), 20);
}

TEST_P(Values16Test, LoadSPfromHL)
{
    auto value = GetParam();
    
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xF9));
    
    EXPECT_CALL(regs, get(Reg16::HL)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(Reg16::SP, value));
    
    EXPECT_EQ(decoder.decodeOne(), 6);
}

TEST_P(Values16IdxTest, LoadSPfromIdx)
{
    auto [value, idx] = GetParam();
    auto [reg, prefix] = idx;
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0xF9));
    
    EXPECT_CALL(regs, get(reg)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(Reg16::SP, value));
    
    EXPECT_EQ(decoder.decodeOne(), 10);
}

TEST_P(QQRegTest, Push)
{
    auto [reg, idx] = GetParam();
    
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xC5 | idx));
    
    EXPECT_CALL(prim, push(reg));
    
    EXPECT_EQ(decoder.decodeOne(), 11);
}

TEST_P(QQIdxRegTest, Push)
{
    auto [reg, prefix] = GetParam();
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0xE5));
    
    EXPECT_CALL(prim, push(reg));
    
    EXPECT_EQ(decoder.decodeOne(), 15);
}

TEST_P(QQRegTest, Pop)
{
    auto [reg, idx] = GetParam();
    
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xC1 | idx));
    
    EXPECT_CALL(prim, pop(reg));
    
    EXPECT_EQ(decoder.decodeOne(), 10);
}

TEST_P(QQIdxRegTest, Pop)
{
    auto [reg, prefix] = GetParam();
    
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(prefix))
        .WillOnce(Return(0xE1));
    
    EXPECT_CALL(prim, pop(reg));
    
    EXPECT_EQ(decoder.decodeOne(), 14);
}
}
