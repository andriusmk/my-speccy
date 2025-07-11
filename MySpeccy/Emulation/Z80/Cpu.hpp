//
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
#include "Interfaces/IRegisters.hpp"
#include "CpuState.hpp"

#include <cstdint>
#include <cstdlib>
#include <optional>

namespace Z80 {

class Cpu
{
public:
    Cpu(IBus& memory, IBus& io, CpuState* const extState = nullptr)
      : memory{memory}, io{io}, state(extState != nullptr ? *extState : internalState),
    test{0x4000}
    {
        testFun();
        for (int i = 0; i < 192 * 32 + 24 * 32; i++)
        {
            writeRandomByte();
        }
    }
    
    Cpu(const Cpu&) = delete;
    
    void reset()
    {
    }
    
    void testFun()
    {
        for (int i = 0x4000; i < 0x4000 + 192 * 32; i++)
        {
            memory.write(i, 0);
        }
        
        for (int i = 0x4000 + 192 * 32; i < 0x4000 + 192 * 32 + 24 * 32; i++)
        {
            memory.write(i, 0x38);
        }
        
        // test = 0x4000;
    }
    
    int executeOne()
    {
        return 4;
    }

    
    
    void setInterrupt()
    {
        
    }
    
    void writeRandomByte()
    {
        memory.write(test, std::rand());
        const int key = io.read(0xFE);
        if ((key & 0x1F) != 0x1F)
        {
            io.write(0xFE, test);
        }
        ++test;
        if (test >= 0x4000 + 192 * 32 + 24 * 32)
        {
            reset();
        }
    }
    
    void clearIterrupt()
    {
    }
    
    void triggerNMI()
    {
    }
    
private:
    CpuState internalState;
    IBus& memory;
    IBus& io;
    CpuState& state;
    std::uint16_t test;
};

}
