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
#include "DecoderTest.hpp"

namespace Z80
{

INSTANTIATE_TEST_SUITE_P(, DecoderFlagsTest, Values(0x00, 0xFF));
INSTANTIATE_TEST_SUITE_P(, Values16Test, ValuesIn(values16));
INSTANTIATE_TEST_SUITE_P(, Values16IdxTest, Combine(ValuesIn(values16), ValuesIn(idxRegs)));

} // namespace Z80
