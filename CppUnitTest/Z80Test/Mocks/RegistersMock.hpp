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

#include "Z80/Interfaces/IRegisters.hpp"

#include <gmock/gmock.h>

namespace Z80
{

class RegistersMock : public IRegisters
{
  public:
    MOCK_METHOD(int, get, (const Reg8), (const, final, override));
    MOCK_METHOD(int, get, (const Reg16), (const, final, override));
    MOCK_METHOD(void, set, (const Reg8, int), (final, override));
    MOCK_METHOD(void, set, (const Reg16, int), (final, override));
};
} // namespace Z80
