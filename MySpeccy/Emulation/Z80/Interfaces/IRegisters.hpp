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

#include <cstdint>
#include <cstddef>

namespace Z80 {

enum class Reg8 : std::uint8_t {
    C = 0, B, E, D, L, H, Flags, A, IXL, IXH, R, I, IYL, IYH, Size
};

enum class Reg16 : std::uint8_t {
    BC = 0, DE, HL, AF, IX, IR, IY, PC, SP, BC_, DE_, HL_, AF_, WZ, Misc, Size
};

enum class Flag : std::uint8_t {
    C = 0x01,
    N = 0x02,
    P = 0x04,
    V = 0x04,
    X = 0x08,
    H = 0x10,
    Y = 0x20,
    Z = 0x40,
    S = 0x80
};

struct RegisterShortcuts
{
    static constexpr auto B = Reg8::B;
    static constexpr auto C = Reg8::C;
    static constexpr auto D = Reg8::D;
    static constexpr auto E = Reg8::E;
    static constexpr auto H = Reg8::H;
    static constexpr auto L = Reg8::L;
    static constexpr auto A = Reg8::A;
    static constexpr auto I = Reg8::I;
    static constexpr auto R = Reg8::R;
    static constexpr auto IXH = Reg8::IXH;
    static constexpr auto IXL = Reg8::IXL;
    static constexpr auto IYH = Reg8::IYH;
    static constexpr auto IYL = Reg8::IYL;
    
    static constexpr auto BC = Reg16::BC;
    static constexpr auto DE = Reg16::DE;
    static constexpr auto HL = Reg16::HL;
    static constexpr auto AF = Reg16::AF;
    static constexpr auto IR = Reg16::IR;
    static constexpr auto IX = Reg16::IX;
    static constexpr auto IY = Reg16::IY;
    static constexpr auto PC = Reg16::PC;
    static constexpr auto SP = Reg16::SP;
    
    static constexpr auto CF = Flag::C;
    static constexpr auto NF = Flag::N;
    static constexpr auto PF = Flag::P;
    static constexpr auto VF = Flag::V;
    static constexpr auto XF = Flag::X;
    static constexpr auto HF = Flag::H;
    static constexpr auto YF = Flag::Y;
    static constexpr auto ZF = Flag::Z;
    static constexpr auto SF = Flag::S;
};

class IRegisters
{
public:
    virtual ~IRegisters() = default;
    
    virtual int get(const Reg8) const = 0;
    virtual int get(const Reg16) const = 0;
    virtual void set(const Reg8, int) = 0;
    virtual void set(const Reg16, int) = 0;
};

class Flags
{
public:
    Flags(IRegisters& regs)
        : regs{regs},
          flags{static_cast<std::uint8_t>(regs.get(Reg8::Flags))}
    {
    }
    
    ~Flags()
    {
        regs.set(Reg8::Flags, flags);
    }
    template<typename... Args>
    static uint8_t combine(Args... args)
    {
        uint8_t result = 0;
        ((result |= static_cast<uint8_t>(args)), ...);
        
        return result;
    }

    template<typename... Args>
    void setFrom(int value, Args... args)
    {
        uint8_t mask = combine(args...);
        
        flags = (flags & ~mask) | (value & mask);
    }
    
    template<typename... Args>
    void setIf(int value, Args... args)
    {
        setFrom((value != 0) * 0xFF, args...);
    }

    template<typename... Args>
    void set(Args... args)
    {
        setFrom(0xFF, args...);
    }
    
    template<typename... Args>
    void clear(Args... args)
    {
        setFrom(0x00, args...);
    }

private:
    IRegisters& regs;
    std::uint8_t flags;
};

static constexpr Reg8 low(Reg16 reg)
{
    return Reg8(static_cast<uint8_t>(reg) << 1);
}

static constexpr Reg8 high(Reg16 reg)
{
    return Reg8((static_cast<uint8_t>(reg) << 1) + 1);
}

}
