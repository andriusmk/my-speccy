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

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <span>

class Memory : public IBus
{
  public:
    using ByteSpan = std::span<std::uint8_t>;

    static constexpr std::size_t romSize{0x4000};
    static constexpr std::size_t ramSize{0xC000};
    static constexpr std::size_t screenOffset{0};
    static constexpr std::size_t screenSize{0x5B00};
    static constexpr std::size_t attrOffset{192 * 32};
    static constexpr std::size_t attrSize{24 * 32};

    Memory() : screenBus_{std::span{ram}.subspan(screenOffset, screenSize)}
    {
    }

    Memory(const Memory&) = delete;

    void loadRom(std::span<const std::uint8_t> content)
    {
        const auto copySize = std::min(content.size(), rom.size());
        std::copy(content.begin(), content.begin() + copySize, rom.begin());
    }

    int read(int addr) const final override
    {
        addr &= 0xFFFF;
        if (addr < romSize)
        {
            return rom[addr];
        }
        return ram[addr - romSize];
    }

    void write(int addr, int data) final override
    {
        addr &= 0xFFFF;
        if (addr >= romSize)
        {
            ram[addr - romSize] = data;
        }
    }

    const IBus& screenBus()
    {
        return screenBus_;
    }

  private:
    class ScreenBus : public IBus
    {
      public:
        ScreenBus(const ByteSpan span) : span(span)
        {
        }

        ScreenBus(const ScreenBus&) = delete;

        int read(int addr) const final override
        {
            return span[addr];
        }

        void write(int addr, int data) final override
        {
        }

      private:
        ByteSpan span;
    };

    std::array<std::uint8_t, romSize> rom;
    std::array<std::uint8_t, ramSize> ram;
    ScreenBus screenBus_;
};
