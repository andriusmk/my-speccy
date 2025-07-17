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

class IdxMain : public IdxBase
{
public:
    IdxMain(Parts& parts) : IdxBase{parts}
    {
    }
    
    IdxMain(const IdxMain&) = delete;
    
    void halt() final override
    {
        parts.prim.halt();
        addTstates(4);
    }
    
    int indirectFromHL() final override
    {
        addTstates(7);
        return parts.prim.getIndirect(Reg16::HL);
    }
    
    void indirectToHL(int value) final override
    {
        parts.prim.setIndirect(Reg16::HL, value);
        addTstates(7);
    }
    
    void indirectToHLfromN() final override
    {
        const auto n = parts.prim.fetch8();
        indirectToHL(n);
        addTstates(10);
    }

private:
    int getReg(Reg8 reg) final override
    {
        return parts.regs.get(reg);
    }
    
    void setReg(Reg8 reg, int value) final override
    {
        parts.regs.set(reg, value);
    }
    
    int getIdx() final override
    {
        return parts.regs.get(Reg16::HL);
    }
    
    void setIdx(int value) final override
    {
        parts.regs.set(Reg16::HL, value);
    }
    
};

}
