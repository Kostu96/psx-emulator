#include "gpu.h"
#include "opengl/renderer.h"

#include <iostream>

uint32_t GPU::load32(uint32_t offset) const
{
    switch (offset) {
    case 0:
        std::cerr << "Temp handled read from GPUREAD\n";
        return 0;
    case 4:
        // TODO: temp
        // apply WAs to GPUSTAT register
        uint32_t status = m_status.word;
        status &= ~(1 << 19); // zero out bit 19 - vertical resolution
        status |= 1 << 26; // set bit 26 - ready to receive command
        status |= 1 << 27; // set bit 27 - ready to send VRAM to CPU
        status |= 1 << 28; // set bit 28 - ready to receive DMA block
        status |= 0 << 31; // bit 31 should changed based on interlacing
        return status;
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
    if (m_gp0RemainigWords == 0) {
        std::function<void()> func;
        uint8_t remaining = 1;
        uint32_t opcode = (value >> 24) & 0xFF;
        switch (opcode) {
        case 0x00: func = std::bind(&GPU::NOP, this, value); break;
        case 0x01: func = std::bind(&GPU::ClearCache, this, value); break;
        case 0x28: func = std::bind(&GPU::RenderOpaqueMonochromeQuad, this, value); remaining = 5; break;
        case 0x2C: func = std::bind(&GPU::RenderOpaqueTexturedQuadWithBlending, this, value); remaining = 9; break;
        case 0x30: func = std::bind(&GPU::RenderOpaqueShadedTriangle, this, value); remaining = 6; break;
        case 0x38: func = std::bind(&GPU::RenderOpaqueShadedQuad, this, value); remaining = 8; break;
        case 0xA0: func = std::bind(&GPU::CopyRectangleToVRAM, this, value); remaining = 3; break;
        case 0xC0: func = std::bind(&GPU::CopyRectangleFromVRAM, this, value); remaining = 3; break;
        case 0xE1: func = std::bind(&GPU::DrawMode, this, value); break;
        case 0xE2: func = std::bind(&GPU::TextureWindow, this, value); break;
        case 0xE3: func = std::bind(&GPU::SetDrawingAreaTL, this, value); break;
        case 0xE4: func = std::bind(&GPU::SetDrawingAreaBR, this, value); break;
        case 0xE5: func = std::bind(&GPU::SetDrawingOffset, this, value); break;
        case 0xE6: func = std::bind(&GPU::MaskBitSetting, this, value); break;
        default:
            std::cerr << "Unhandled gp0 command: " << std::hex << opcode << std::dec << '\n';
            //abort();
        }

        m_gp0RemainigWords = remaining;
        m_gp0CommandFunc = func;
        m_gp0CommandBuffer.clear();
    }
    
    m_gp0RemainigWords -= 1;

    switch (m_gp0Mode) {
    case GP0Mode::Command:
        m_gp0CommandBuffer.push(value);
        if (m_gp0RemainigWords == 0)
            m_gp0CommandFunc();
        break;
    case GP0Mode::ImageLoad:
        if (m_gp0RemainigWords == 0)
            m_gp0Mode = GP0Mode::Command;
        break;
    }
}

void GPU::NOP(uint32_t /*instruction*/)
{
    // GP0(00h)
}

void GPU::ClearCache(uint32_t instruction)
{
    // GP0(01h)
    std::cerr << "Unimplemented GP0(01h) command!\n";
}

void GPU::RenderOpaqueMonochromeQuad(uint32_t instruction)
{
    // GP0(28h)
    Renderer::Vertex vertices[6];

    vertices[0].x = m_gp0CommandBuffer[1] & 0xFFFF;
    vertices[0].y = (m_gp0CommandBuffer[1] >> 16) & 0xFFFF;
    vertices[1].x = m_gp0CommandBuffer[2] & 0xFFFF;
    vertices[1].y = (m_gp0CommandBuffer[2] >> 16) & 0xFFFF;
    vertices[2].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[2].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;

    vertices[3].x = m_gp0CommandBuffer[2] & 0xFFFF;
    vertices[3].y = (m_gp0CommandBuffer[2] >> 16) & 0xFFFF;
    vertices[4].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[4].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;
    vertices[5].x = m_gp0CommandBuffer[4] & 0xFFFF;
    vertices[5].y = (m_gp0CommandBuffer[4] >> 16) & 0xFFFF;

    vertices[5].r = vertices[4].r = vertices[3].r = vertices[2].r = vertices[1].r = vertices[0].r = m_gp0CommandBuffer[0] & 0xFF;
    vertices[5].g = vertices[4].g = vertices[3].g = vertices[2].g = vertices[1].g = vertices[0].g = (m_gp0CommandBuffer[0] >> 8) & 0xFF;
    vertices[5].b = vertices[4].b = vertices[3].b = vertices[2].b = vertices[1].b = vertices[0].b = (m_gp0CommandBuffer[0] >> 16) & 0xFF;
    vertices[5].a = vertices[4].a = vertices[3].a = vertices[2].a = vertices[1].a = vertices[0].a = 0xFF;

    m_renderer.pushQuad(vertices, sizeof(vertices));
}

void GPU::RenderOpaqueTexturedQuadWithBlending(uint32_t instruction)
{
    // GP0(2Ch)
    Renderer::Vertex vertices[6];

    vertices[0].x = m_gp0CommandBuffer[1] & 0xFFFF;
    vertices[0].y = (m_gp0CommandBuffer[1] >> 16) & 0xFFFF;
    vertices[1].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[1].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;
    vertices[2].x = m_gp0CommandBuffer[5] & 0xFFFF;
    vertices[2].y = (m_gp0CommandBuffer[5] >> 16) & 0xFFFF;

    vertices[3].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[3].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;
    vertices[4].x = m_gp0CommandBuffer[5] & 0xFFFF;
    vertices[4].y = (m_gp0CommandBuffer[5] >> 16) & 0xFFFF;
    vertices[5].x = m_gp0CommandBuffer[7] & 0xFFFF;
    vertices[5].y = (m_gp0CommandBuffer[7] >> 16) & 0xFFFF;

    vertices[5].r = vertices[4].r = vertices[3].r = vertices[2].r = vertices[1].r = vertices[0].r = m_gp0CommandBuffer[0] & 0xFF;
    vertices[5].g = vertices[4].g = vertices[3].g = vertices[2].g = vertices[1].g = vertices[0].g = (m_gp0CommandBuffer[0] >> 8) & 0xFF;
    vertices[5].b = vertices[4].b = vertices[3].b = vertices[2].b = vertices[1].b = vertices[0].b = (m_gp0CommandBuffer[0] >> 16) & 0xFF;
    vertices[5].a = vertices[4].a = vertices[3].a = vertices[2].a = vertices[1].a = vertices[0].a = 0xFF;

    m_renderer.pushQuad(vertices, sizeof(vertices));
}

void GPU::RenderOpaqueShadedTriangle(uint32_t instruction)
{
    // GP0(30h)
    Renderer::Vertex vertices[3];

    vertices[0].x = m_gp0CommandBuffer[1] & 0xFFFF;
    vertices[0].y = (m_gp0CommandBuffer[1] >> 16) & 0xFFFF;
    vertices[1].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[1].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;
    vertices[2].x = m_gp0CommandBuffer[5] & 0xFFFF;
    vertices[2].y = (m_gp0CommandBuffer[5] >> 16) & 0xFFFF;

    vertices[0].r = m_gp0CommandBuffer[0] & 0xFF;
    vertices[0].g = (m_gp0CommandBuffer[0] >> 8) & 0xFF;
    vertices[0].b = (m_gp0CommandBuffer[0] >> 16) & 0xFF;
    vertices[0].a = 0xFF;
    vertices[1].r = m_gp0CommandBuffer[2] & 0xFF;
    vertices[1].g = (m_gp0CommandBuffer[2] >> 8) & 0xFF;
    vertices[1].b = (m_gp0CommandBuffer[2] >> 16) & 0xFF;
    vertices[1].a = 0xFF;
    vertices[2].r = m_gp0CommandBuffer[4] & 0xFF;
    vertices[2].g = (m_gp0CommandBuffer[4] >> 8) & 0xFF;
    vertices[2].b = (m_gp0CommandBuffer[4] >> 16) & 0xFF;
    vertices[2].a = 0xFF;

    m_renderer.pushTriangle(vertices, sizeof(vertices));
}

void GPU::RenderOpaqueShadedQuad(uint32_t instruction)
{
    // GP0(38h)
    Renderer::Vertex vertices[6];

    vertices[0].x = m_gp0CommandBuffer[1] & 0xFFFF;
    vertices[0].y = (m_gp0CommandBuffer[1] >> 16) & 0xFFFF;
    vertices[1].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[1].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;
    vertices[2].x = m_gp0CommandBuffer[5] & 0xFFFF;
    vertices[2].y = (m_gp0CommandBuffer[5] >> 16) & 0xFFFF;

    vertices[0].r = m_gp0CommandBuffer[0] & 0xFF;
    vertices[0].g = (m_gp0CommandBuffer[0] >> 8) & 0xFF;
    vertices[0].b = (m_gp0CommandBuffer[0] >> 16) & 0xFF;
    vertices[0].a = 0xFF;
    vertices[1].r = m_gp0CommandBuffer[2] & 0xFF;
    vertices[1].g = (m_gp0CommandBuffer[2] >> 8) & 0xFF;
    vertices[1].b = (m_gp0CommandBuffer[2] >> 16) & 0xFF;
    vertices[1].a = 0xFF;
    vertices[2].r = m_gp0CommandBuffer[4] & 0xFF;
    vertices[2].g = (m_gp0CommandBuffer[4] >> 8) & 0xFF;
    vertices[2].b = (m_gp0CommandBuffer[4] >> 16) & 0xFF;
    vertices[2].a = 0xFF;

    vertices[3].x = m_gp0CommandBuffer[3] & 0xFFFF;
    vertices[3].y = (m_gp0CommandBuffer[3] >> 16) & 0xFFFF;
    vertices[4].x = m_gp0CommandBuffer[5] & 0xFFFF;
    vertices[4].y = (m_gp0CommandBuffer[5] >> 16) & 0xFFFF;
    vertices[5].x = m_gp0CommandBuffer[7] & 0xFFFF;
    vertices[5].y = (m_gp0CommandBuffer[7] >> 16) & 0xFFFF;

    vertices[3].r = m_gp0CommandBuffer[2] & 0xFF;
    vertices[3].g = (m_gp0CommandBuffer[2] >> 8) & 0xFF;
    vertices[3].b = (m_gp0CommandBuffer[2] >> 16) & 0xFF;
    vertices[3].a = 0xFF;
    vertices[4].r = m_gp0CommandBuffer[4] & 0xFF;
    vertices[4].g = (m_gp0CommandBuffer[4] >> 8) & 0xFF;
    vertices[4].b = (m_gp0CommandBuffer[4] >> 16) & 0xFF;
    vertices[4].a = 0xFF;
    vertices[5].r = m_gp0CommandBuffer[6] & 0xFF;
    vertices[5].g = (m_gp0CommandBuffer[6] >> 8) & 0xFF;
    vertices[5].b = (m_gp0CommandBuffer[6] >> 16) & 0xFF;
    vertices[5].a = 0xFF;

    m_renderer.pushQuad(vertices, sizeof(vertices));
}

void GPU::CopyRectangleToVRAM(uint32_t instruction)
{
    // GP0(A0h)
    uint32_t res = m_gp0CommandBuffer[2];
    uint32_t width = res & 0xFFFF;
    uint32_t height = res >> 16;
    uint32_t imgSize = width * height;
    imgSize = (imgSize + 1) & ~1;
    m_gp0RemainigWords = imgSize / 2;
    m_gp0Mode = GP0Mode::ImageLoad;
}

void GPU::CopyRectangleFromVRAM(uint32_t instruction)
{
    // GP0(C0h)
    std::cerr << "Unimplemented GP0(C0h) command!\n";
    uint32_t res = m_gp0CommandBuffer[2];
    uint32_t width = res & 0xFFFF;
    uint32_t height = res >> 16;
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

    // TODO: temporary hack
    m_renderer.swap();
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
    case 0x01: ResetCommandBuffer(value); break;
    case 0x02: ResetGPUIRQ(value); break;
    case 0x03: DisplayDisable(value); break;
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
    m_gp0CommandBuffer.clear();
    m_gp0CommandFunc = nullptr;
    m_gp0Mode = GP0Mode::Command;
    m_gp0RemainigWords = 0;
    // TODO: Unfinished!
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
