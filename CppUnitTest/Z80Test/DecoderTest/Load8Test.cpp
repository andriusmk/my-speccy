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

#include <cstdint>
#include <tuple>

namespace Z80
{

using ::testing::A;
using ::testing::AnyNumber;
using ::testing::Combine;
using ::testing::InSequence;
using ::testing::Values;
using ::testing::ValuesIn;

using RegId = std::pair<uint8_t, Reg8>;
using IndexRegId = std::pair<uint8_t, Reg16>;

using OneRegParam = std::tuple<RegId, uint8_t>;
using TwoRegParam = std::tuple<RegId, RegId, uint8_t>;
using IndexedRegParam = std::tuple<RegId, IndexRegId, uint8_t>;
using IndexedTwoRegParam = std::tuple<RegId, RegId, IndexRegId, uint8_t>;

class DecoderTwoRegTest : public DecoderTest, public ::testing::WithParamInterface<TwoRegParam>
{
};

class DecoderSrcRegTest : public DecoderTest, public ::testing::WithParamInterface<OneRegParam>
{
};

class DecoderDstRegTest : public DecoderTest, public ::testing::WithParamInterface<OneRegParam>
{
};

class DecoderIdxSrcRegTest : public DecoderTest, public ::testing::WithParamInterface<IndexedRegParam>
{
};

class DecoderIdxDstRegTest : public DecoderTest, public ::testing::WithParamInterface<IndexedRegParam>
{
};

class DecoderPrefixedTwoRegTest : public DecoderTest, public ::testing::WithParamInterface<IndexedTwoRegParam>
{
};

static constexpr std::array<RegId, 7> srcRegs{std::make_pair(0x00, Reg8::B), std::make_pair(0x01, Reg8::C),
                                              std::make_pair(0x02, Reg8::D), std::make_pair(0x03, Reg8::E),
                                              std::make_pair(0x04, Reg8::H), std::make_pair(0x05, Reg8::L),
                                              std::make_pair(0x07, Reg8::A)};

static constexpr std::array<RegId, 7> dstRegs{std::make_pair(0x00, Reg8::B), std::make_pair(0x08, Reg8::C),
                                              std::make_pair(0x10, Reg8::D), std::make_pair(0x18, Reg8::E),
                                              std::make_pair(0x20, Reg8::H), std::make_pair(0x28, Reg8::L),
                                              std::make_pair(0x38, Reg8::A)};

static constexpr std::array<RegId, 7> srcRegsDD{std::make_pair(0x00, Reg8::B),   std::make_pair(0x01, Reg8::C),
                                                std::make_pair(0x02, Reg8::D),   std::make_pair(0x03, Reg8::E),
                                                std::make_pair(0x04, Reg8::IXH), std::make_pair(0x05, Reg8::IXL),
                                                std::make_pair(0x07, Reg8::A)};

static constexpr std::array<RegId, 7> dstRegsDD{std::make_pair(0x00, Reg8::B),   std::make_pair(0x08, Reg8::C),
                                                std::make_pair(0x10, Reg8::D),   std::make_pair(0x18, Reg8::E),
                                                std::make_pair(0x20, Reg8::IXH), std::make_pair(0x28, Reg8::IXL),
                                                std::make_pair(0x38, Reg8::A)};

static constexpr std::array<RegId, 7> srcRegsFD{std::make_pair(0x00, Reg8::B),   std::make_pair(0x01, Reg8::C),
                                                std::make_pair(0x02, Reg8::D),   std::make_pair(0x03, Reg8::E),
                                                std::make_pair(0x04, Reg8::IYH), std::make_pair(0x05, Reg8::IYL),
                                                std::make_pair(0x07, Reg8::A)};

static constexpr std::array<RegId, 7> dstRegsFD{std::make_pair(0x00, Reg8::B),   std::make_pair(0x08, Reg8::C),
                                                std::make_pair(0x10, Reg8::D),   std::make_pair(0x18, Reg8::E),
                                                std::make_pair(0x20, Reg8::IYH), std::make_pair(0x28, Reg8::IYL),
                                                std::make_pair(0x38, Reg8::A)};

static constexpr std::array<uint8_t, 4> regValues{0x00, 0x55, 0xAA, 0xFF};

static constexpr std::array<IndexRegId, 2> indexRegs{std::make_pair(0xDD, Reg16::IX), std::make_pair(0xFD, Reg16::IY)};

INSTANTIATE_TEST_SUITE_P(, DecoderTwoRegTest, Combine(ValuesIn(srcRegs), ValuesIn(dstRegs), ValuesIn(regValues)));

INSTANTIATE_TEST_SUITE_P(, DecoderSrcRegTest, Combine(ValuesIn(srcRegs), ValuesIn(regValues)));

INSTANTIATE_TEST_SUITE_P(, DecoderDstRegTest, Combine(ValuesIn(dstRegs), ValuesIn(regValues)));

INSTANTIATE_TEST_SUITE_P(, DecoderIdxSrcRegTest, Combine(ValuesIn(srcRegs), ValuesIn(indexRegs), ValuesIn(regValues)));

INSTANTIATE_TEST_SUITE_P(, DecoderIdxDstRegTest, Combine(ValuesIn(dstRegs), ValuesIn(indexRegs), ValuesIn(regValues)));

INSTANTIATE_TEST_SUITE_P(PrefixDD, DecoderPrefixedTwoRegTest,
                         Combine(ValuesIn(srcRegsDD), ValuesIn(dstRegsDD), Values(std::make_pair(0xDD, Reg16::IX)),
                                 ValuesIn(regValues)));

INSTANTIATE_TEST_SUITE_P(PrefixFD, DecoderPrefixedTwoRegTest,
                         Combine(ValuesIn(srcRegsFD), ValuesIn(dstRegsFD), Values(std::make_pair(0xFD, Reg16::IY)),
                                 ValuesIn(regValues)));

TEST_F(DecoderTest, Nop)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x00));

    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_F(DecoderTest, Nops)
{
    EXPECT_CALL(prim, fetchM1()).Times(3).WillRepeatedly(Return(0x00));

    EXPECT_EQ(decoder.decodeOne(), 4);
    EXPECT_EQ(decoder.decodeOne(), 4);
    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_F(DecoderTest, Halt)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x76));
    EXPECT_CALL(prim, halt());

    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_P(DecoderTwoRegTest, LdRfromR)
{
    const auto [src, dst, value] = GetParam();
    const auto [srcIdx, srcReg] = src;
    const auto [dstIdx, dstReg] = dst;

    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0b01000000 | srcIdx | dstIdx));
    EXPECT_CALL(regs, get(srcReg)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(dstReg, value));

    EXPECT_EQ(decoder.decodeOne(), 4);
}

TEST_P(DecoderSrcRegTest, LdHLfromR)
{
    const auto [src, value] = GetParam();
    const auto [idx, reg] = src;

    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0b01110000 | idx));
    EXPECT_CALL(regs, get(reg)).WillOnce(Return(value));
    EXPECT_CALL(prim, setIndirect(Reg16::HL, value));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_P(DecoderDstRegTest, LdRfromHL)
{
    const auto [dst, value] = GetParam();
    const auto [idx, reg] = dst;

    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0b01000110 | idx));
    EXPECT_CALL(prim, getIndirect(Reg16::HL)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(reg, value));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_P(DecoderIdxSrcRegTest, LdIXdFromR)
{
    const auto [src, ix, value] = GetParam();
    const auto [srcRegIdx, srcReg] = src;
    const auto [prefix, idxReg] = ix;

    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(prefix)).WillOnce(Return(0b01110000 | srcRegIdx));

    EXPECT_CALL(regs, get(srcReg)).WillOnce(Return(value));
    EXPECT_CALL(prim, setIndexed(idxReg, value));

    EXPECT_EQ(decoder.decodeOne(), 19);
}

TEST_P(DecoderIdxDstRegTest, LdRfromIXd)
{
    const auto [dst, ix, value] = GetParam();
    const auto [dstRegIdx, dstReg] = dst;
    const auto [prefix, idxReg] = ix;

    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(prefix)).WillOnce(Return(0b01000110 | dstRegIdx));

    EXPECT_CALL(prim, getIndexed(idxReg)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(dstReg, value));

    EXPECT_EQ(decoder.decodeOne(), 19);
}

TEST_P(DecoderPrefixedTwoRegTest, Test)
{
    const auto [src, dst, ix, value] = GetParam();
    const auto [srcIdx, srcReg] = src;
    const auto [dstIdx, dstReg] = dst;
    const auto [prefix, idxReg] = ix;

    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(prefix)).WillOnce(Return(0b01000000 | srcIdx | dstIdx));

    EXPECT_CALL(regs, get(srcReg)).WillOnce(Return(value));
    EXPECT_CALL(regs, set(dstReg, value));

    EXPECT_EQ(decoder.decodeOne(), 8);
}

TEST_F(DecoderTest, Tstates)
{
    EXPECT_CALL(prim, fetchM1())
        .WillOnce(Return(0x00))
        .WillOnce(Return(0x41))
        .WillOnce(Return(0xFD))
        .WillOnce(Return(0x7E))
        .WillOnce(Return(0xDD))
        .WillOnce(Return(0x78));

    EXPECT_CALL(regs, get(A<Reg8>())).WillRepeatedly(Return(0));
    EXPECT_CALL(regs, set(A<Reg8>(), _)).Times(AnyNumber());
    EXPECT_CALL(prim, getIndexed(_)).WillRepeatedly(Return(0));

    EXPECT_EQ(decoder.decodeOne(), 4);
    EXPECT_EQ(decoder.decodeOne(), 4);
    EXPECT_EQ(decoder.decodeOne(), 19);
    EXPECT_EQ(decoder.decodeOne(), 8);
}

TEST_F(DecoderTest, PrefixedHaltDD)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xDD)).WillOnce(Return(0x76));

    EXPECT_CALL(prim, halt()).Times(0);

    EXPECT_EQ(decoder.decodeOne(), 8);
}

TEST_F(DecoderTest, PrefixedHaltFD)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xFD)).WillOnce(Return(0x76));

    EXPECT_CALL(prim, halt()).Times(0);

    EXPECT_EQ(decoder.decodeOne(), 8);
}

TEST_F(DecoderTest, LoadN)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0b00111110));
    EXPECT_CALL(prim, fetch8()).WillOnce(Return(0));
    EXPECT_CALL(regs, set(Reg8::A, 0));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_F(DecoderTest, LoadN1)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0b00110110));
    EXPECT_CALL(prim, fetch8()).WillOnce(Return(0x55));
    EXPECT_CALL(prim, setIndirect(Reg16::HL, 0x55));

    EXPECT_EQ(decoder.decodeOne(), 10);
}

TEST_F(DecoderTest, LoadN2)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xDD)).WillOnce(Return(0b00100110));
    EXPECT_CALL(prim, fetch8()).WillOnce(Return(0xAA));
    EXPECT_CALL(regs, set(Reg8::IXH, 0xAA));

    EXPECT_EQ(decoder.decodeOne(), 11);
}

TEST_F(DecoderTest, LoadN3)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xFD)).WillOnce(Return(0x36));

    EXPECT_CALL(prim, fetch8()).WillOnce(Return(0x42)).WillOnce(Return(0xAA));
    EXPECT_CALL(prim, setIndexed(Reg16::IY, 0x42, 0xAA));

    EXPECT_EQ(decoder.decodeOne(), 19);
}

TEST_F(DecoderTest, LoadIndirectBCtoA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x0A));

    EXPECT_CALL(prim, getIndirect(Reg16::BC)).WillOnce(Return(0x42));
    EXPECT_CALL(regs, set(Reg8::A, 0x42));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_F(DecoderTest, LoadIndirectDEtoA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x1A));

    EXPECT_CALL(prim, getIndirect(Reg16::DE)).WillOnce(Return(0x43));
    EXPECT_CALL(regs, set(Reg8::A, 0x43));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_F(DecoderTest, LoadIndirectNNtoA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x3A));

    EXPECT_CALL(prim, getIndirectNN()).WillOnce(Return(0x45));
    EXPECT_CALL(regs, set(Reg8::A, 0x45));

    EXPECT_EQ(decoder.decodeOne(), 13);
}

TEST_F(DecoderTest, LoadIndirectBCfromA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x02));

    EXPECT_CALL(regs, get(Reg8::A)).WillOnce(Return(0x42));
    EXPECT_CALL(prim, setIndirect(Reg16::BC, 0x42));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_F(DecoderTest, LoadIndirectDEfromA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x12));

    EXPECT_CALL(regs, get(Reg8::A)).WillOnce(Return(0x43));
    EXPECT_CALL(prim, setIndirect(Reg16::DE, 0x43));

    EXPECT_EQ(decoder.decodeOne(), 7);
}

TEST_F(DecoderTest, LoadIndirectNNfromA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0x32));

    EXPECT_CALL(regs, get(Reg8::A)).WillOnce(Return(0x45));
    EXPECT_CALL(prim, setIndirectNN(0x45));

    EXPECT_EQ(decoder.decodeOne(), 13);
}

TEST_F(DecoderTest, LoadIfromA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xED)).WillOnce(Return(0x47));

    EXPECT_CALL(regs, get(Reg8::A)).WillOnce(Return(0x49));
    EXPECT_CALL(regs, set(Reg8::I, 0x49));

    EXPECT_EQ(decoder.decodeOne(), 9);
}

TEST_F(DecoderTest, LoadRfromA)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xED)).WillOnce(Return(0x4F));

    EXPECT_CALL(regs, get(Reg8::A)).WillOnce(Return(0x33));
    EXPECT_CALL(regs, set(Reg8::R, 0x33));

    EXPECT_EQ(decoder.decodeOne(), 9);
}

TEST_P(DecoderFlagsTest, LoadAFromI)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xED)).WillOnce(Return(0x57));

    EXPECT_CALL(regs, get(Reg8::I)).WillOnce(Return(0xA1));
    EXPECT_CALL(regs, set(Reg8::A, 0xA1));
    EXPECT_CALL(prim, getIff2()).WillOnce(Return(1));

    EXPECT_EQ(decoder.decodeOne(), 9);

    verifySet(Flag::S, Flag::P, Flag::Y);
    verifyCleared(Flag::N, Flag::X, Flag::H, Flag::Z);
    verifyUnchanged(Flag::C);
}

TEST_P(DecoderFlagsTest, LoadAfromI2)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xED)).WillOnce(Return(0x57));

    EXPECT_CALL(regs, get(Reg8::I)).WillOnce(Return(0x09));
    EXPECT_CALL(regs, set(Reg8::A, 0x09));
    EXPECT_CALL(prim, getIff2()).WillOnce(Return(0));

    EXPECT_EQ(decoder.decodeOne(), 9);

    verifySet(Flag::X);
    verifyCleared(Flag::N, Flag::P, Flag::H, Flag::Y, Flag::Z, Flag::S);
    verifyUnchanged(Flag::C);
}

TEST_P(DecoderFlagsTest, LoadAfromR)
{
    EXPECT_CALL(prim, fetchM1()).WillOnce(Return(0xED)).WillOnce(Return(0x5F));

    EXPECT_CALL(regs, get(Reg8::R)).WillOnce(Return(0x09));
    EXPECT_CALL(regs, set(Reg8::A, 0x09));
    EXPECT_CALL(prim, getIff2()).WillOnce(Return(1));

    EXPECT_EQ(decoder.decodeOne(), 9);

    verifySet(Flag::P, Flag::X);
    verifyCleared(Flag::N, Flag::H, Flag::Y, Flag::Z, Flag::S);
    verifyUnchanged(Flag::C);
}

} // namespace Z80
