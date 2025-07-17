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

#include "IdxBase.hpp"
#include "Parts.hpp"

namespace Z80 {

class IdxIdx : public IdxBase
{
public:
    IdxIdx(Parts& parts, Reg16 reg = Reg16::IX) : IdxBase{parts}, idxReg{reg}
    {
    }
    
    IdxIdx(const IdxIdx&) = delete;
    
    Reg16 operator=(Reg16 newReg)
    {
        idxReg = newReg;
        return idxReg;
    }
    
    void halt() final override
    {
        addTstates(4);
    }

    int indirectFromHL() final override
    {
        addTstates(15);
        return parts.prim.getIndexed(idxReg);
    }
    
    void indirectToHL(int value) final override
    {
        addTstates(15);
        parts.prim.setIndexed(idxReg, value);
    }
    
    void indirectToHLfromN() final override
    {
        const auto d = parts.prim.fetch8();
        const auto n = parts.prim.fetch8();
        
        parts.prim.setIndexed(idxReg, d, n);
        addTstates(15);
    }

private:
    int getReg(Reg8 reg) final override
    {
        switch (reg)
        {
            case Reg8::H:
                return parts.regs.get(high(idxReg));
                
            case Reg8::L:
                return parts.regs.get(low(idxReg));
                
            default:
                return parts.regs.get(reg);
        }
    }
    
    void setReg(Reg8 reg, int value) final override
    {
        switch (reg)
        {
            case Reg8::H:
                parts.regs.set(high(idxReg), value);
                break;
                
            case Reg8::L:
                parts.regs.set(low(idxReg), value);
                break;

            default:
                parts.regs.set(reg, value);
                break;
        }
    }
    
    int getIdx() final override
    {
        return parts.regs.get(idxReg);
    }

    void setIdx(int value) final override
    {
        parts.regs.set(idxReg, value);
    }

    Reg16 idxReg;
};

}
