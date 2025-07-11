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

#include "Parts.hpp"
#include "IdxMain.hpp"
#include "IdxIdx.hpp"
#include "Tools.hpp"

#include <optional>
#include <array>
#include <functional>

namespace Z80 {

class Decoder : public RegisterShortcuts, public IDecoder
{
public:
    Decoder(Parts& parts)
    : parts(parts), idxMain{parts}, idxIdx{parts}
    {
    }
    Decoder(const Decoder&) = delete;
    
    int decodeOne() final override
    {
        idxMain.takeTstates();
        idxIdx.takeTstates();
        
        return decodeOne(0);
    }
    
private:
    int decodeOne(int tstates)
    {
        auto opcode = parts.prim.fetchM1();
        
        switch (opcode)
        {
            case 0x00:
                return 4 + tstates;
                
            case 0x02:
                parts.prim.setIndirect(BC, parts.regs.get(Reg8::A));
                return 7 + tstates;

            case 0x0A:
                parts.regs.set(Reg8::A, parts.prim.getIndirect(BC));
                return 7 + tstates;
                
            case 0x12:
                parts.prim.setIndirect(DE, parts.regs.get(Reg8::A));
                return 7 + tstates;

            case 0x1A:
                parts.regs.set(Reg8::A, parts.prim.getIndirect(DE));
                return 7 + tstates;
                
            case 0x32:
                parts.prim.setIndirectNN(parts.regs.get(Reg8::A));
                return 13 + tstates;

            case 0x3A:
                parts.regs.set(Reg8::A, parts.prim.getIndirectNN());
                return 13 + tstates;

            case 0x76:
                parts.prim.halt();
                return 4 + tstates;
                
            case 0xED:
                return decodeED(4 + tstates);
        }
        
        return decodeCommon(opcode, tstates, idxMain);
    }
    
    int decodeCommon(int opcode, int tstates, IIdxVariant& idx)
    {
        switch (opcode)
        {
            case 0x36:
                idx.indirectToHLfromN();
                return idx.takeTstates() + tstates;
                
            case 0xCB:
                return decodeCB(tstates + 4, idx);

            case 0xDD: {
                idxIdx = IX;
                const auto newOpcode = parts.prim.fetchM1();
                return decodeCommon(newOpcode, tstates + 4, idxIdx);
            }
                
            case 0xFD: {
                idxIdx = IY;
                const auto newOpcode = parts.prim.fetchM1();
                return decodeCommon(newOpcode, tstates + 4, idxIdx);
            }
                
            case 0x76: // catching HALT because it overlaps with LD
                return 4 + tstates;
        }
        
        if (ld_hl_r(opcode))
        {
            const auto src = regTab[opcode & 7].value();
            idx.indirectToHL(parts.regs.get(src));
            
            return idx.takeTstates() + tstates;
        }
        
        if (ld_r_hl(opcode))
        {
            const auto dst = regTab[(opcode >> 3) & 7].value();
            parts.regs.set(dst, idx.indirectFromHL());
            
            return idx.takeTstates() + tstates;
        }
        
        if (ld_r_r(opcode))
        {
            const auto dst = regTab[(opcode >> 3) & 7].value();
            const auto src = regTab[opcode & 7].value();
            
            idx.load(dst, src);
            
            return idx.takeTstates() + tstates;
        }
        
        if (ld_r_n(opcode))
        {
            const auto dst = regTab[(opcode >> 3) & 7].value();
            idx.loadN(dst);
            
            return idx.takeTstates() + tstates;
        }

        return 4 + tstates;
    }
    
    int decodeED(int tstates)
    {
        const auto opcode = parts.prim.fetchM1();
        
        switch (opcode)
        {
            case 0x47:
                parts.regs.set(I, parts.regs.get(A));
                return 5 + tstates;
                
            case 0x4F:
                parts.regs.set(R, parts.regs.get(A));
                return 5 + tstates;
            
            case 0x57: {
                Flags flags{parts.regs};
                const auto value = parts.regs.get(I);
                parts.regs.set(A, value);
                flags.setFrom(value, SF, XF, YF);
                flags.clear(HF, NF);
                flags.setIf(value == 0, ZF);
                flags.setIf(parts.prim.getIff2(), PF);
                
                return 5 + tstates;
            }
                
            case 0x5F: {
                Flags flags{parts.regs};
                const auto value = parts.regs.get(R);
                parts.regs.set(A, value);
                flags.setFrom(value, SF, XF, YF);
                flags.clear(HF, NF);
                flags.setIf(value == 0, ZF);
                flags.setIf(parts.prim.getIff2(), PF);
                
                return 5 + tstates;
            }
        }
        
        return 4 + tstates;
    }
    
    int decodeCB(int tstates, IIdxVariant& idx)
    {
        return tstates;
    }

    struct OpMask
    {
        uint8_t op;
        uint8_t mask;
        bool operator()(int v) const
        {
            return (v & mask) == op;
        }
    };
    
    static constexpr auto ld_r_r  = OpMask{ 0b01000000, 0b11000000 };
    static constexpr auto ld_hl_r = OpMask{ 0b01110000, 0b11111000 };
    static constexpr auto ld_r_hl = OpMask{ 0b01000110, 0b11000111 };
    static constexpr auto ld_r_n = OpMask{ 0b00000110, 0b11000111 };

    static constexpr std::array<std::optional<Reg8>, 8> regTab{
        B, C, D, E, H, L, std::nullopt, A
    };
    
    Parts& parts;
    IdxMain idxMain;
    IdxIdx idxIdx;
};

}
