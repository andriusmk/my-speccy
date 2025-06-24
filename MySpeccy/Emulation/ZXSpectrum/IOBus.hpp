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

#include "Interfaces/IBus.hpp"
#include "Interfaces/IBorderCtrl.hpp"

#include <cstdint>
#include <array>

class IOBus : public IBus
{
public:
    IOBus(IBorderCtrl& borderCtrl) : borderCtrl{borderCtrl}, columns{}
    {
    }
    
    IOBus(const IOBus&) = delete;
    
    int read(int addr) const final override
    {
        int result = 0xFF;
        if ((addr & 1) == 0)
        {
            for (int i = 0; i < 8; i++)
            {
                if ((addr & 0x100) == 0)
                {
                    result &= ~columns[i];
                }
                addr >>= 1;
            }
        }
        return result;
    }
    
    void write(int addr, int data) final override
    {
        if ((addr & 1) == 0)
        {
            borderCtrl.setBorder(data & 7);
        }
    }
    
    void keyDown(uint32_t key)
    {
        columns[key >> 5] |= key & 0x1F;
    }
    
    void keyUp(uint32_t key)
    {
        columns[key >> 5] &= ~(key & 0x1F);
    }

private:
    IBorderCtrl& borderCtrl;
    std::array<std::uint8_t, 8> columns;
};
