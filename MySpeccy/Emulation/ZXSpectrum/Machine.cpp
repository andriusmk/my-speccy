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

#include "Machine.hpp"
#include "Screen.hpp"
#include "Memory.hpp"
#include "IOBus.hpp"
#include "Z80/Cpu.hpp"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using Z80::Cpu;

class Machine::Impl : public IVSyncCtrl
{
public:
    Impl()
    : screen{memory.screenBus(), *this},
    ioBus{screen},
    cpu{memory, ioBus},
    audioSamples{0},
    intCycles{0}
    {
        std::srand(std::time({}));
    }
    
    FrameInfo frameInfo() const {
        return Screen::frameInfo;
    }

    void vSync(int cycles) final override
    {
        isVSync = 1;
        vSyncCycles = std::min<int>(cycles, 255);
    }
    
    void processFrame(FrameData& data)
    {
        isVSync = 0;
        int lastCycles = 0;
        while (isVSync == 0)
        {
            lastCycles = cpu.executeOne();
            if (intCycles > 0)
            {
                intCycles = std::max<int>(static_cast<int>(intCycles) - lastCycles, 0);
                if (intCycles == 0)
                {
                    cpu.clearIterrupt();
                }
            }
            screen.runCycles(lastCycles);
        }
        const int vSyncBefore = lastCycles - vSyncCycles;
        intCycles = std::max<int>(32 - vSyncBefore, 0);
        if (intCycles > 0) {
            cpu.setInterrupt();
        }
        
        data.pixels = screen.pixels();
        std::fill_n(data.audioBuffer.buffer, 882, 0);
        data.audioSamplesProduced = 882;
    }
    
    void keyDown(uint32_t key)
    {
        ioBus.keyDown(key);
    }
    
    void keyUp(uint32_t key)
    {
        ioBus.keyUp(key);
    }
    
    void loadROM(const uint8_t* data, uint32_t size)
    {
        memory.loadRom(std::span<const uint8_t>(data, size));
    }

private:
    Memory memory;
    Screen screen;
    IOBus ioBus;
    Cpu cpu;
    std::uint32_t audioSamples;
    std::uint8_t vSyncCycles;
    std::uint8_t intCycles;
    uint8_t isVSync : 1;
};

Machine::Machine() : impl{std::make_unique<Machine::Impl>()}
{
}

Machine::Machine(Machine&& other) noexcept = default;

Machine::~Machine() = default;

FrameInfo Machine::frameInfo() const
{
    return impl->frameInfo();
}

void Machine::processFrame(FrameData& data)
{
    impl->processFrame(data);
}

void Machine::keyDown(uint32_t key)
{
    impl->keyDown(key);
}

void Machine::keyUp(uint32_t key)
{
    impl->keyUp(key);
}

void Machine::loadROM(const uint8_t* data, uint32_t size)
{
    impl->loadROM(data, size);
}
