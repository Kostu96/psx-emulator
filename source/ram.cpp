#include "ram.h"

#include <iostream>

RAM::RAM()
{
    m_data = new uint8_t[SIZE];
}

RAM::~RAM()
{
    delete[] m_data;
}

uint32_t RAM::load32(uint32_t offset) const
{
    uint8_t b0 = m_data[offset];
    uint8_t b1 = m_data[offset + 1];
    uint8_t b2 = m_data[offset + 2];
    uint8_t b3 = m_data[offset + 3];

    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void RAM::store32(uint32_t offset, uint32_t value)
{
    uint8_t b0 = value & 0xFF;
    uint8_t b1 = (value >> 8) & 0xFF;
    uint8_t b2 = (value >> 16) & 0xFF;
    uint8_t b3 = (value >> 24) & 0xFF;

    m_data[offset] = b0;
    m_data[offset + 1] = b1;
    m_data[offset + 2] = b2;
    m_data[offset + 3] = b3;
}

const AddressRange RAM::RANGE{ 0xA0000000, 2 * 1024 * 1024 };
