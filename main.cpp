#include <iostream>
#include <fstream>
#include <vector>

struct Instruction
{
    uint32_t opcode;

    Instruction(uint32_t inst) : opcode(inst) {}

    uint32_t function() const { return opcode >> 26; }
    uint32_t target() const { return (opcode >> 16) & 0x1F; }
    uint32_t immediate() const { return opcode & 0xFFFF; }
};

struct AddressRange
{
    uint32_t start;
    uint32_t size;

    bool contains(uint32_t address) const
    {
        return (address >= start) && (address < start + size);
    }
};

struct BIOS
{
    const AddressRange range{ 0xBFC00000, 512 * 1024 };
    std::vector<char> data;

    BIOS()
    {
        const char* BIOSFilename = "SCPH-1001.bin";
        std::ifstream fin(BIOSFilename, std::ios::binary | std::ios::ate);
        if (!fin.is_open()) {
            std::cerr << "Failed to open file: " << BIOSFilename << '\n';
        }

        std::streamsize size = fin.tellg();
        if (size != 512 * 1024) {
            std::cerr << "BIOS file should have 512KB. Size read is " << size << '\n';
        }

        data.resize(size);
        fin.seekg(0);
        if (!fin.read(data.data(), size)) {
            std::cerr << "Failed to read from: " << BIOSFilename << "\nFile size was: " << size << '\n';
        }

        fin.close();
    }

    uint32_t load32(uint32_t offset) const
    {
        uint8_t b0 = data[offset];
        uint8_t b1 = data[offset + 1];
        uint8_t b2 = data[offset + 2];
        uint8_t b3 = data[offset + 3];

        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }
};

struct Registers
{
    uint32_t regs[32]{};

    void set(uint32_t reg, uint32_t value)
    {
        regs[reg] = value;
        regs[0] = 0;
    }

    uint32_t get(uint32_t reg) const { return regs[reg]; }
};

struct Interconnect // TODO: rename to MemoryMap?
{
    BIOS bios;

    uint32_t load32(uint32_t address) const
    {
        if (address % 4 != 0)
            std::cerr << "Unaligned access when loading from: "
                      << std::hex << address << std::dec << '\n';

        if (bios.range.contains(address))
            return bios.load32(address - bios.range.start);

        std::cerr << "Unhandled access when loading from: "
                  << std::hex << address << std::dec << '\n';
    }

    void store32(uint32_t address, uint32_t value)
    {
        if (address % 4 != 0)
            std::cerr << "Unaligned access when storing to: "
                      << std::hex << address << std::dec << '\n';

        std::cerr << "Unhandled access when storing to: "
                  << std::hex << address << std::dec << '\n';
    }
};

struct CPU
{
    uint32_t PC = 0xBFC00000;
    Registers regs;
    Interconnect memoryMap;

    void runNextInstruction()
    {
        uint32_t inst = load32(PC);
        PC += 4;
        decodeAndExecute(inst);
    }

    void decodeAndExecute(Instruction instruction)
    {
        uint32_t func = instruction.function();
        switch (func) // TODO: change to array of function pointers
        {
        case 0b001111:
            opLUI(instruction);
            break;
        case 0b001101:
            opORI(instruction);
            break;
        case 0b101011:
            opSW(instruction);
            break;
        default:
            std::cerr << "Unhandled instruction: " << std::hex << instruction.opcode << std::dec << '\n';
        }
    }

    uint32_t load32(uint32_t address)
    {
        return memoryMap.load32(address);
    }

    void store32(uint32_t address, uint32_t value)
    {
        memoryMap.store32(address, value);
    }

    void opLUI(Instruction instruction)
    {
        regs.set(instruction.target(), instruction.immediate() << 16);
    }

    void opORI(Instruction instruction)
    {
        uint32_t target = instruction.target();
        uint32_t value = regs.get(target);
        value |= instruction.immediate();
        regs.set(target, value);
    }

    void opSW(Instruction instruction)
    {
        store32(regs.get(instruction.target()) + instruction.immediate(), instruction.target()); // ?
    }
};

int main()
{
    CPU cpu;
    for (;;)
        cpu.runNextInstruction();

    return 0;
}
