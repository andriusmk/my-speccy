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
#include <cmath>
#include <array>

#include "Interfaces/IBus.hpp"
#include "Interfaces/API.hpp"
#include "Interfaces/IBorderCtrl.hpp"
#include "Interfaces/IVSyncCtrl.hpp"

class Screen : public IBorderCtrl
{
public:
    using Pixel = uint16_t;
    
    static constexpr int colorBits{5};
    static constexpr int nonBrightLevelLinear{0x15};
    
    static constexpr int colorMask{(1 << colorBits) - 1};
    static constexpr int nonBrightMask{
        (nonBrightLevelLinear) |
        (nonBrightLevelLinear << (colorBits)) |
        (nonBrightLevelLinear << (colorBits * 2)) };
    
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
            const Pixel borderColor = makeColor(border) & nonBrightMask;
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
        
//        const Pixel inkPaper[] {
//            colors[((attrs >> 3) & 0x0F)],
//            colors[(attrs & 7) | ((attrs & 0x40) >> 3)]
//        };
        
        const int flashState = flash & (attrs >> 7) & 1;
        
        for (int i = 0; i < 8; i++)
        {
            buffer[octetBase + i] = makePixel(attrs, ((pixs & 0x80) >> 7) ^ flashState);
            pixs <<= 1;
        }
    }
    
    static Pixel makeColor(int index)
    {
        Pixel result = (index & 1) * colorMask;
        result |= ((index & 2) * colorMask) << (colorBits * 2 - 1);
        result |= ((index & 4) * colorMask) << (colorBits - 2);

        return result;
    }
    
    static Pixel makePixel(int attr, int value)
    {
        Pixel color = makeColor(attr & 7) * value;
        color |= makeColor((attr >> 3) & 7) * (value ^ 1);
        
        Pixel result = color & nonBrightMask;
        result |= color * ((attr & 0x40) >> 6);
        
        return result;
    }
    
    std::array<Pixel, frameWidth * frameHeight> buffer;
    const IBus& memory;
    IVSyncCtrl& vSyncCtrl;
    int cycles;
    std::uint8_t frame;
    std::uint8_t border;
    std::uint8_t flash;
    
    static constexpr int attributeBase = 3 * 8 * 8 * 32;
    
};
