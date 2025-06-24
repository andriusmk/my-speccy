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
#include <array>

#include "Interfaces/IBus.hpp"
#include "Interfaces/API.hpp"
#include "Interfaces/IBorderCtrl.hpp"
#include "Interfaces/IVSyncCtrl.hpp"

class Screen : public IBorderCtrl
{
public:
    using Pixel = uint32_t;
    
    static constexpr int lineBlankCycles{48};
    static constexpr int totalLineCycles{224};
    static constexpr int frameBlankCycles{5488};
    static constexpr int totalFrameCycles{70000};
    static constexpr int pixelsPerCycle{2};
    static constexpr int cyclesToFirstByte{14336};
    
    static constexpr int borderWidth{48};
    static constexpr int borderHeight{48};
    static constexpr int frameWidth{352};
    static constexpr int frameHeight{288};
    static constexpr int screenWidth{256};
    static constexpr int screenHeight{192};
    
    static constexpr int topLeftCornerCycles{cyclesToFirstByte - borderWidth / pixelsPerCycle - totalLineCycles * borderHeight};
    static constexpr int lineCycles{frameWidth / pixelsPerCycle};
    static constexpr int bottomRightCornerCycles{topLeftCornerCycles + totalLineCycles * frameHeight - lineBlankCycles};
    static constexpr int octetCycles{8 / pixelsPerCycle};

    static constexpr FrameInfo frameInfo {
        .width = frameWidth,
        .height = frameHeight,
        .bytesPerRow = frameWidth * sizeof(Pixel)
    };
    
    Screen(const IBus& memory, IVSyncCtrl& vSync)
    : buffer{}, vSyncCtrl{vSync}, memory{memory}, cycles{0}, frame{0}, border{7}, flash{0}
    {
    }
    
    Screen(const Screen&) = delete;
    
    Pixel* pixels()
    {
        return &buffer[0];
    }
    
    void setBorder(int border) final override
    {
        this->border = border & 7;
    }
    
    void runCycles(const int cyclesToRun)
    {
        const int finalCycles = cycles + cyclesToRun;
        
        if (finalCycles > totalFrameCycles)
        {
            const int toEnd = totalFrameCycles - cycles;
            const int inNextFrame = cyclesToRun - toEnd;
            
            runCycles(toEnd);
            
            cycles = 0;
            frame++;
            if (frame >= 50) {
                frame = 0;
            }
            flash = frame < 25;
            vSyncCtrl.vSync(toEnd);
            
            runCycles(inNextFrame);
            
            return;
        }
        
        
        if (finalCycles >= topLeftCornerCycles + octetCycles &&
            cycles >= topLeftCornerCycles &&
            cycles < bottomRightCornerCycles)
        {
            const int screenOctet = (cycles - topLeftCornerCycles) / octetCycles;
            const int finalOctet = (finalCycles - topLeftCornerCycles) / octetCycles;
            
            drawOctets(screenOctet, finalOctet);
        }
        
        cycles = finalCycles;
    }

private:
    void drawOctets(int screenOctet, const int finalOctet)
    {
        int line = screenOctet / (totalLineCycles / octetCycles);
        int lineOctet = screenOctet % (totalLineCycles / octetCycles);
        
        while (screenOctet < finalOctet)
        {
            if (lineOctet >= (lineCycles / octetCycles)) {
                ++line;
                if (line >= frameHeight) break;
                screenOctet = line * (totalLineCycles / octetCycles);
                lineOctet = 0;
                continue;
            }
            
            drawOctet(line, lineOctet);
            
            ++screenOctet;
            ++lineOctet;
        }
    }
    
    void drawOctet(int line, int lineOctet)
    {
        const int octetBase = line * frameWidth + lineOctet * 8;
        
        if (line < borderHeight ||
            line >= borderHeight + screenHeight ||
            lineOctet < borderWidth / 8 ||
            lineOctet >= borderWidth / 8 + screenWidth / 8)
        {
            const Pixel borderColor = colors[border];
            for (int i = 0; i < 8; i++)
            {
                buffer[octetBase + i] = borderColor;
            }
            return;
        }
        
        const int screenLine = line - borderHeight;
        const int charInLine = lineOctet - borderWidth / 8;
        
        const int third = (screenLine >> 6) & 3;
        const int lineInChar = screenLine & 7;
        const int charLine = screenLine >> 3;
    
        const int pixelAddr = (third << 11) | (lineInChar << 8) | ((charLine & 7) << 5) | charInLine;
        const int attributeAddr = attributeBase + charLine * 32 + charInLine;
        
        int pixs = memory.read(pixelAddr);
        const int attrs = memory.read(attributeAddr);
        
        const Pixel inkPaper[] {
            colors[((attrs >> 3) & 7)],
            colors[(attrs & 7) | ((attrs & 0x40) >> 3)]
        };
        
        const int flashState = flash & (attrs >> 7) & 1;
        
        for (int i = 0; i < 8; i++)
        {
            buffer[octetBase + i] = inkPaper[((pixs & 0x80) != 0) ^ flashState];
            pixs <<= 1;
        }
    }
    
    std::array<Pixel, frameWidth * frameHeight> buffer;
    const IBus& memory;
    IVSyncCtrl& vSyncCtrl;
    int cycles;
    std::uint8_t frame;
    std::uint8_t border;
    std::uint8_t flash;
    
    static constexpr int attributeBase = 3 * 8 * 8 * 32;
    
    static constexpr std::array<Pixel, 16> colors {
        0x000000,
        0x0000E6,
        0xE60000,
        0xE600E6,
        0x00E600,
        0x00E6E6,
        0xE6E600,
        0xE6E6E6,
        0x000000,
        0x0000FF,
        0xFF0000,
        0xFF00FF,
        0x00FF00,
        0x00FFFF,
        0xFFFF00,
        0xFFFFFF
    };
};
