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

namespace Z80 {

inline static int isEvenParity8(int value)
{
    value ^= (value >> 4);
    value ^= (value >> 2);
    value ^= (value >> 1);
    
    return (value & 1) == 0;
}

inline static int isOverflow(int a, int b, int result)
{
    const int opSignsEqual = ~(a ^ b);
    const int resultSignDiffers = a ^ result;
    
    return opSignsEqual & resultSignDiffers;
}

inline static int isOverflow8(int a, int b, int result)
{
    return isOverflow(a, b, result) & 0x80;
}

inline static int isOverflow16(int a, int b, int result)
{
    return isOverflow(a, b, result) & 0x8000;
}

inline static int isCarry(int a, int b, int result)
{
    return a ^ b ^ result;
}

inline static int makeCarry8(int a, int b, int result)
{
    const auto cf = isCarry(a, b, result);
    return (cf & 0x10) | (cf >> 8);
}

inline static int makeCarry16(int a, int b, int result)
{
    const auto cf = isCarry(a, b, result) >> 8;
    return (cf & 0x10) | (cf >> 8);
}

}
