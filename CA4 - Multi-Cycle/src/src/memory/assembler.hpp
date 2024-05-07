#ifndef ASSEMBLER_HPP_INCLUDE
#define ASSEMBLER_HPP_INCLUDE

#include <array>
#include <charconv>
#include <cstdint>
#include <unordered_map>

#include "strutils.hpp"

struct Asm {
    using MachineInst = std::int16_t;

    // clang-format off
    struct Opcode {
        enum type {
            add    = 0b1000,
            sub    = 0b1000,
            bitAnd = 0b1000,
            bitOr  = 0b1000,
            bitNot = 0b1000,
            nop    = 0b1000,
            mvTo   = 0b1000,
            mvFrom = 0b1000,
            addi   = 0b1100,
            subi   = 0b1101,
            andi   = 0b1110,
            ori    = 0b1111,
            load   = 0b0000,
            store  = 0b0001,
            jump   = 0b0010,
            branch = 0b0100,
        };
    };

    struct RFunc {
        enum type {
            add    = 0b000000100,
            sub    = 0b000001000,
            bitAnd = 0b000010000,
            bitOr  = 0b000100000,
            bitNot = 0b001000000,
            nop    = 0b010000000,
            mvTo   = 0b000000001,
            mvFrom = 0b000000010,
        };
    };

    inline static const std::unordered_map<std::string, Opcode::type> opcMap {
        {"add",    Opcode::add},
        {"sub",    Opcode::sub},
        {"and",    Opcode::bitAnd},
        {"or",     Opcode::bitOr},
        {"not",    Opcode::bitNot},
        {"nop",    Opcode::nop},
        {"mvto",   Opcode::mvTo},
        {"mvfrom", Opcode::mvFrom},
        {"addi",   Opcode::addi},
        {"subi",   Opcode::subi},
        {"andi",   Opcode::andi},
        {"ori",    Opcode::ori},
        {"load",   Opcode::load},
        {"store",  Opcode::store},
        {"jump",   Opcode::jump},
        {"branch", Opcode::branch},
    };

    inline static const std::unordered_map<std::string, RFunc::type> rfuncMap {
        {"add", RFunc::add},
        {"sub", RFunc::sub},
        {"and", RFunc::bitAnd},
        {"or",  RFunc::bitOr},
        {"not", RFunc::bitNot},
        {"nop", RFunc::nop},
        {"mvto",   RFunc::mvTo},
        {"mvfrom", RFunc::mvFrom},
    };
    // clang-format on

    static MachineInst assemble(const std::vector<std::string>& inst,
                                const std::unordered_map<std::string, int>& labelMap,
                                const std::size_t instCount) {
        using namespace std::string_literals;

        auto itr = opcMap.find(inst[0]);
        if (itr == opcMap.end()) {
            throw std::invalid_argument("Invalid instruction: "s + inst[0]);
        }

        Opcode::type opc = itr->second;
        std::array<MachineInst, 3> instInt {};
        instInt[0] = opc;

        bool labelExists = false;

        for (std::size_t i = 1, pos; i < inst.size(); ++i) {
            const auto begin = inst[i].data();
            const auto end = begin + inst[i].size();

            if (inst[i][0] == 'R') { // R12
                auto res = std::from_chars(begin + 1, end, instInt[i]);
                if (res.ec == std::errc::invalid_argument || res.ptr != end) goto label;
            }
            else if ((pos = inst[i].find('(')) != std::string::npos) { // 4(R2)
                std::from_chars(begin, begin + pos, instInt[i]);
                std::from_chars(begin + pos + 2, end - 1, instInt[i + 1]);
            }
            else if (std::isdigit(inst[i][0]) || inst[i][0] == '-') { // 24
                std::from_chars(begin, end, instInt[i]);
            }
            else {
            label:
                auto itr = labelMap.find(inst[i]);
                if (itr == labelMap.end()) {
                    throw std::invalid_argument("Invalid label: "s + inst[i]);
                }
                instInt[i] = itr->second;
                labelExists = true;
            }
        }

        if (inst.size() > 3) {
            throw std::invalid_argument("Invalid operands for instruction: "s + inst[0]);
        }

        MachineInst mInst {};
        switch (opc) {
        case Opcode::add:
            instInt[2] = rfuncMap.find(inst[0])->second;
            mInst = ctype(instInt);
            break;
        case Opcode::addi:
        case Opcode::subi:
        case Opcode::andi:
        case Opcode::ori:
            mInst = dtype(instInt);
            break;
        case Opcode::load:
        case Opcode::store:
        case Opcode::jump:
            mInst = atype(instInt);
            break;
        case Opcode::branch:
            mInst = btype(instInt);
            break;
        default:
            throw std::invalid_argument("Invalid opcode: "s + std::to_string(opc));
        }

        return mInst;
    }

    static MachineInst atype(const std::array<MachineInst, 3> inst) {
        // load store branch
        // opc[4] adr[12]
        MachineInst result = inst[0] << 12; // opc
        result += inst[1] & 0xFFF;          // adr
        return result;
    }

    static MachineInst btype(const std::array<MachineInst, 3> inst) {
        // branch
        // opc[4] reg[3] adr[9]
        MachineInst result = inst[0] << 12; // opc
        result += inst[1] << 9;             // reg
        result += inst[2] & 0x1FF;          // adr
        return result;
    }

    static MachineInst ctype(const std::array<MachineInst, 3> inst) {
        // add sub and or not nop mvto mvfrom
        // opc[4] reg[3] func[9]
        MachineInst result = inst[0] << 12; // opc
        result += inst[1] << 9;             // reg
        result += inst[2] & 0x1FF;          // func
        return result;
    }

    static MachineInst dtype(const std::array<MachineInst, 3> inst) {
        // addi subi andi ori
        // opc[4] imm[12]
        MachineInst result = inst[0] << 12; // opc
        result += inst[1] & 0xFFF;          // imm
        return result;
    }
};

#endif // ASSEMBLER_HPP_INCLUDE
