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

namespace Z80
{

class IIdxVariant
{
  public:
    virtual ~IIdxVariant() = default;

    virtual int takeTstates() = 0;

    virtual void halt() = 0;

    virtual int indirectFromHL() = 0;
    virtual void indirectToHL(int) = 0;
    virtual void indirectToHLfromN() = 0;

    virtual void indirectNNtoHL() = 0;
    virtual void indirectNNfromHL() = 0;

    virtual void load(Reg8 dst, Reg8 src) = 0;
    virtual void loadN(Reg8 dst) = 0;
    virtual void loadNN() = 0;

    virtual int getIdx() = 0;
    virtual void setIdx(int) = 0;

    virtual Reg16 get() const = 0;
};

} // namespace Z80
