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

#include "Z80/Interfaces/IIdxVariant.hpp"

namespace Z80 {

class IdxBase : public IIdxVariant
{
public:
    IdxBase(Parts& parts) : parts{parts}, tstates{0}
    {
    }
    
    virtual ~IdxBase() = default;
    
    int takeTstates() final override
    {
        int result = tstates;
        tstates = 0;
        return result;
    }
    
    void load(Reg8 dst, Reg8 src) final override
    {
        setReg(dst, getReg(src));
        addTstates(4);
    }
    
    void loadN(Reg8 dst) final override
    {
        auto value = parts.prim.fetch8();
        setReg(dst, value);
        addTstates(7);
    }
    
    void loadNN() final override
    {
        setIdx(parts.prim.fetch16());
        addTstates(10);
    }

    void indirectNNfromHL() final override
    {
        parts.prim.write16(parts.prim.fetch16(), getIdx());
        addTstates(16);
    }
    
    void indirectNNtoHL() final override
    {
        setIdx(parts.prim.read16(parts.prim.fetch16()));
        addTstates(16);
    }

protected:
    void addTstates(int value)
    {
        tstates = value;
    }
    
private:
    virtual int getReg(Reg8) = 0;
    virtual void setReg(Reg8, int) = 0;

protected:
    Parts& parts;

private:
    int tstates;
};
}
