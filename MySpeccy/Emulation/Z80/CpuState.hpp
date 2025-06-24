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

namespace Z80 {


struct CpuState {
    struct SwitchableSet
    {
        uint16_t regs[3];
    };
    union {
        struct {
            uint16_t BC;
            uint16_t DE;
            uint16_t HL;
            uint16_t AF;
            uint16_t IR;
            uint16_t IX;
            uint16_t IY;
        };
        struct {
            uint8_t C;
            uint8_t B;
            uint8_t E;
            uint8_t D;
            uint8_t L;
            uint8_t H;
            uint8_t Flags;
            uint8_t A;
            uint8_t R;
            uint8_t I;
            uint8_t IXL;
            uint8_t IXH;
            uint8_t IYL;
            uint8_t IYH;
        };
        uint8_t regs[8];
        uint16_t regPairs[4];
        SwitchableSet main;
    };
    SwitchableSet alt;
    uint16_t altAF;
    uint16_t PC;
    uint16_t SP;
    uint16_t WZ;
    uint8_t IFF1 :1;
    uint8_t IFF2 :1;
};

}
