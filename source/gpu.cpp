#include "gpu.h"

#include <iostream>

uint32_t GPU::load32(uint32_t offset) const
{
    switch (offset) {
    case 0:
        std::cerr << "Temp handled read from GPUREAD\n";
        return 0;
    //case 4:
    //    abort();
    //    return 0;
    }

    std::cerr << "Store to ivalid GPU location: " << std::hex << offset << std::dec << '\n';
    abort();
}

void GPU::store32(uint32_t offset, uint32_t value)
{
    switch (offset) {
    case 0:
        gp0write(value);
        return;
    case 4:
        gp1write(value);
        return;
    }
    
    std::cerr << "Store to ivalid GPU location: " << std::hex << offset << std::dec << '\n';
    abort();
}

void GPU::gp0write(uint32_t value)
{
    if (m_remainigCommandWords == 0) {
        std::function<void()> func;
        uint8_t remaining = 1;
        uint32_t opcode = (value >> 24) & 0xFF;
        switch (opcode) {
        case 0x00: func = std::bind(&GPU::NOP, this, value); break;
        case 0x28: func = std::bind(&GPU::RenderOpaqueMonoChromeQuad, this, value); remaining = 5; break;
        case 0xE1: func = std::bind(&GPU::DrawMode, this, value); break;
        case 0xE2: func = std::bind(&GPU::TextureWindow, this, value); break;
        case 0xE3: func = std::bind(&GPU::SetDrawingAreaTL, this, value); break;
        case 0xE4: func = std::bind(&GPU::SetDrawingAreaBR, this, value); break;
        case 0xE5: func = std::bind(&GPU::SetDrawingOffset, this, value); break;
        case 0xE6: func = std::bind(&GPU::MaskBitSetting, this, value); break;
        default:
            std::cerr << "Unhandled gp0 command: " << std::hex << opcode << std::dec << '\n';
            abort();
        }

        m_remainigCommandWords = remaining;
        m_commandFunc = func;
        m_commandBuffer.clear();
    }

    m_commandBuffer.push(value);
    m_remainigCommandWords -= 1;

    if (m_remainigCommandWords == 0)
        m_commandFunc();
}

void GPU::NOP(uint32_t /*instruction*/)
{
    // GP0(00h)
}

void GPU::RenderOpaqueMonoChromeQuad(uint32_t instruction)
{
    // GP0(28h)

}

void GPU::DrawMode(uint32_t instruction)
{
    // GP0(E1h)
    m_status.word &= ~0x3FF;
    m_status.word |= instruction & 0x3FF;
    m_status.fields.textureDisable = (instruction >> 11) & 1;
    m_rectangleTextureXFlip = (instruction >> 12) & 1;
    m_rectangleTextureYFlip = (instruction >> 13) & 1;
}

void GPU::TextureWindow(uint32_t instruction)
{
    // GP0(E2h)
    m_textureWindowMaskX = instruction & 0x1F;
    m_textureWindowMaskY = (instruction >> 5) & 0x1F;
    m_textureWindowOffsetX = (instruction >> 10) & 0x1F;
    m_textureWindowOffsetY = (instruction >> 15) & 0x1F;
}

void GPU::SetDrawingAreaTL(uint32_t instruction)
{
    // GP0(E3h)
    m_drawingAreaLeft = instruction & 0x3FF;
    m_drawingAreaTop = (instruction >> 10) & 0x1FF;
}

void GPU::SetDrawingAreaBR(uint32_t instruction)
{
    // GP0(E4h)
    m_drawingAreaRight = instruction & 0x3FF;
    m_drawingAreaBottom = (instruction >> 10) & 0x1FF;
}

void GPU::SetDrawingOffset(uint32_t instruction)
{
    // GP0(E5h)
    uint16_t hw = instruction & 0x7FF;
    if (hw & 0x400)
        hw |= 0xF800;
    m_drawingOffsetX = hw;

    hw = (instruction >> 11) & 0x7FF;
    if (hw & 0x400)
        hw |= 0xF800;
    m_drawingOffsetY = hw;
}

void GPU::MaskBitSetting(uint32_t instruction)
{
    // GP0(E6h)
    m_status.fields.forceSetMask = instruction & 1;
    m_status.fields.preserveMasked = (instruction >> 1) & 1;
}

void GPU::gp1write(uint32_t value)
{
    uint32_t opcode = (value >> 24) & 0xFF;
    switch (opcode) {
    case 0x00: Reset(value); break;
    case 0x04: DMADirection(value); break;
    case 0x05: DisplayAreaStart(value); break;
    case 0x06: HorizontalDisplayRange(value); break;
    case 0x07: VerticalDisplayRange(value); break;
    case 0x08: DisplayMode(value); break;
    default:
        std::cerr << "Unhandled gp1 command: " << std::hex << opcode << std::dec << '\n';
        abort();
    }
}

void GPU::Reset(uint32_t /*instruction*/)
{
    // GP1(00h)
    ResetCommandBuffer(0);
    ResetGPUIRQ(0);
    DisplayDisable(1);
    DMADirection(0);
    DisplayAreaStart(0);
    HorizontalDisplayRange(0xC00200); // x1 = 200h x2 = C00h
    VerticalDisplayRange(0x40010); // y1 = 10h y2 = 100h
    DisplayMode(0);
    DrawMode(0);
    TextureWindow(0);
    SetDrawingAreaTL(0);
    SetDrawingAreaBR(0);
    SetDrawingOffset(0);
    MaskBitSetting(0);
}

void GPU::ResetCommandBuffer(uint32_t /*instruction*/)
{
    // GP1(01h)
    // TODO:
}

void GPU::ResetGPUIRQ(uint32_t /*instruction*/)
{
    // GP1(02h)
    m_status.fields.interrupt = 0;
}

void GPU::DisplayDisable(uint32_t instruction)
{
    // GP1(03h)
    m_status.fields.displayDisable = instruction & 1;
}

void GPU::DMADirection(uint32_t instruction)
{
    // GP1(04h)
    m_status.fields.DMADirection = instruction & 0b11;
}

void GPU::DisplayAreaStart(uint32_t instruction)
{
    // GP1(05h)
    m_vramDisplayStartX = instruction & 0x3FF;
    m_vramDisplayStartY = (instruction >> 10) & 0x1FF;
}

void GPU::HorizontalDisplayRange(uint32_t instruction)
{
    // GP1(06h)
    m_displayHorizontalStart = instruction & 0xFFF;
    m_displayHorizontalEnd = (instruction >> 12) & 0xFFF;
}

void GPU::VerticalDisplayRange(uint32_t instruction)
{
    // GP1(07h)
    m_displayLineStart = instruction & 0x3FF;
    m_displayLineEnd = (instruction >> 10) & 0x3FF;
}

void GPU::DisplayMode(uint32_t instruction)
{
    // GP1(08h)
    m_status.word &= ~0x7E0000;
    m_status.word |= (instruction << 17) & 0x7E0000;
    m_status.fields.horizontalResolution2 = (instruction >> 6) & 1;
    m_status.fields.reverse = (instruction >> 7) & 1;
    if (m_status.fields.reverse) {
        std::cerr << "Reverse flag (bit 14) of GPUSTAT was set!\n";
        abort();
    }
}
