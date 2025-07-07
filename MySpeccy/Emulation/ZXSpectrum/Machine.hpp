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

#include "Interfaces/API.hpp"

class Machine {
public:
    Machine();
    Machine(Machine&&) noexcept;
    
    Machine(const Machine&) = delete;
    Machine& operator=(const Machine&) = delete;
    
    ~Machine();
    
    FrameInfo frameInfo() const;
    void processFrame(FrameData&);
    void keyDown(uint32_t);
    void keyUp(uint32_t);
    void loadROM(const uint8_t*, uint32_t);

private:
    class Impl;
    Impl* impl;
};
