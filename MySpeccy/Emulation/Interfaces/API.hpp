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

struct FrameInfo{
    std::uint16_t width;
    std::uint16_t height;
    std::uint16_t bytesPerRow;
};

struct EmuAudioBuffer
{
    float* buffer;
    std::uint32_t capacity;
};

struct FrameData
{
    EmuAudioBuffer audioBuffer;
    void* pixels;
    std::uint32_t audioSamplesProduced;
};
