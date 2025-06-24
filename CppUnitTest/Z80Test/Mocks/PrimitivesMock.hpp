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

#include "Z80/Interfaces/IPrimitives.hpp"

#include <gmock/gmock.h>

namespace Z80 {

class PrimitivesMock : public IPrimitives
{
public:
    MOCK_METHOD(int, fetchM1, (), (final, override));
    MOCK_METHOD(int, fetch8, (), (final, override));
    MOCK_METHOD(int, fetch16, (), (final, override));
    MOCK_METHOD(void, halt, (), (final, override));
    MOCK_METHOD(void, unhalt, (), (final, override));
    MOCK_METHOD(void, setIndirect, (const Reg16, int), (final, override));
    MOCK_METHOD(int, setIndirectNN, (int), (final, override));
    MOCK_METHOD(int, getIndirect, (const Reg16), (final, override));
    MOCK_METHOD(int, getIndirectNN, (), (final, override));
    MOCK_METHOD(void, setIndexed, (const Reg16, int), (final, override));
    MOCK_METHOD(void, setIndexed, (const Reg16, int, int), (final, override));
    MOCK_METHOD(int, getIndexed, (const Reg16), (final, override));
    MOCK_METHOD(int, getIndexed, (const Reg16, int), (final, override));
    MOCK_METHOD(int, getIff2, (), (const, final, override));
};

}
