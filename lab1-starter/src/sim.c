#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "shell.h"

void process_instruction() {
    /* Fetch the instruction from memory */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    /* Extract the opcode and other fields */
    uint32_t opcode31_26 = (instruction >> 26) & 0x3F;
    uint32_t opcode20_16 = (instruction >> 16) & 0x1F;
    uint32_t opcode5_0 = instruction & 0x3F;
    uint32_t v0 = CURRENT_STATE.REGS[2];

    /* Decode and execute the instruction */
    if (opcode31_26 == 0x01) {
        uint32_t rs = (instruction >> 21) & 0x1F; // Extract source register
        int16_t offset = instruction & 0xFFFF;
        if (offset & 0x8000) // If the sign bit is set
            offset |= 0xFFFF0000;
    
        switch(opcode20_16) { // Opcode from 20 to 16 bits
        case 0x00: // BLTZ
        if ((int32_t)CURRENT_STATE.REGS[rs] < 0) {
            uint32_t target_address = CURRENT_STATE.PC + (offset << 2);
            NEXT_STATE.PC = target_address;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BLTZ\n");
        break;
        case 0x01: // BGEZ
        if ((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
            uint32_t target_address = CURRENT_STATE.PC + (offset << 2);
            NEXT_STATE.PC = target_address;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BGEZ\n");
        break;
        case 0x10: // BLTZAL
        if ((int32_t)CURRENT_STATE.REGS[rs] < 0) {
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; 
            uint32_t target_address = CURRENT_STATE.PC + (offset << 2);

            NEXT_STATE.PC = target_address;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BLTZAL\n");
        break;
        case 0x11: // BGEZAL
        if ((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;

            uint32_t target_address = CURRENT_STATE.PC + (offset << 2);

            NEXT_STATE.PC = target_address;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BGEZAL\n");
        break;
    }
    }
    else if (opcode31_26 == 0x00) {
        uint32_t rs = (instruction >> 21) & 0x1F; // Extract source register
        uint32_t rt = (instruction >> 16) & 0x1F; // Extract destination register
        uint32_t rd = (instruction >> 11) & 0x1F; // Extract source register
        uint32_t sa = (instruction >> 6) & 0x1F; // Extract shift amount
        uint32_t shifted_value = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
        int64_t result = (int64_t)((int32_t)CURRENT_STATE.REGS[rs]) * (int64_t)((int32_t)CURRENT_STATE.REGS[rt]);
        int32_t dividend;
        int32_t divisor;
        int32_t quotient;
        int32_t remainder;
        int32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t rt_value = CURRENT_STATE.REGS[rt];
    
        switch(opcode5_0) { // Opcode from 5 to 0 bits
        case 0x00: // SLL
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
            NEXT_STATE.PC += 4;
            printf("SLL\n");
        break;
        case 0x02: // SRL
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
            NEXT_STATE.PC += 4;
            printf("SRL\n");
        break;
        case 0x03: // SRA
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> sa;
            NEXT_STATE.PC += 4;
            printf("SRA\n");
        break;
        case 0x04: // SLLV
        NEXT_STATE.REGS[rd] = shifted_value;
        NEXT_STATE.PC += 4;
        printf("SLLV\n");
        break;
        case 0x06: // SRLV
        shifted_value = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
        NEXT_STATE.REGS[rd] = shifted_value;
        NEXT_STATE.PC += 4;
        printf("SRLV\n");
        break;
        case 0x07: // SRAV
        shifted_value = (int32_t)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
        NEXT_STATE.REGS[rd] = (uint32_t)shifted_value;
        NEXT_STATE.PC += 4;
        printf("SRAV\n");
        break;
        case 0x08: // JR
        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
        printf("JR\n");
        break;
        case 0x09: // JALR
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
        NEXT_STATE.REGS[rd] = CURRENT_STATE.PC;
        printf("JALR\n");
        break;
        case 0X10: // MFHI
        NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
        NEXT_STATE.PC += 4;
        printf("MFHI\n");
        break;
        case 0x11: // MTHI
        NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC += 4;
        printf("MTHI\n");
        break;
        case 0x12: // MFLO
        NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
        NEXT_STATE.PC += 4;
        printf("MFLO\n");
        break;
        case 0x13: // MTLO
        NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC += 4;
        printf("MTLO\n");
        break;
        case 0x18: // MULT
        NEXT_STATE.HI = (uint32_t)(result >> 32);
        NEXT_STATE.LO = (uint32_t)result;
        NEXT_STATE.PC += 4;
        printf("MULT\n");
        break;
        case 0x19: // MULTU
        result = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
        NEXT_STATE.HI = (uint32_t)(result >> 32);
        NEXT_STATE.LO = (uint32_t)result;
        NEXT_STATE.PC += 4;
        printf("MULTU\n");
        break;
        case 0x20: // ADD
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("ADD\n");
        break;
        case 0x21: // ADDU
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("ADDU\n");
        break;
        case 0x22: // SUB
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("SUB\n");
        break;
        case 0x23: // SUBU
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("SUBU\n");
        break;
        case 0x24: // AND
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("AND\n");
        break;
        case 0x25: // OR
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("OR\n");
        break;
        case 0x26: // XOR
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC += 4;
            printf("XOR\n");
        break;
        case 0x27: // NOR
        result = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
        NEXT_STATE.REGS[rd] = result;
        NEXT_STATE.PC += 4;
        printf("NOR\n");
        break;
        case 0x1A: // DIV
        dividend = CURRENT_STATE.REGS[rs];
        divisor = CURRENT_STATE.REGS[rt];
if (divisor == 0) {
            printf("Error: Division by zero!\n");
            exit(1);
        }
        quotient = dividend / divisor;
        remainder = dividend % divisor;
        NEXT_STATE.LO = quotient;
        NEXT_STATE.HI = remainder;
        NEXT_STATE.PC += 4;
        printf("DIV\n");
        break;
        case 0x1B: // DIVU
        dividend = CURRENT_STATE.REGS[rs];
        divisor = CURRENT_STATE.REGS[rt];
        if (divisor == 0) {
            printf("Error: Division by zero!\n");
            exit(1);
        }
        quotient = dividend / divisor;
        remainder = dividend % divisor;
        NEXT_STATE.LO = quotient;
        NEXT_STATE.HI = remainder;
        NEXT_STATE.PC += 4;
        printf("DIVU\n");
        break;
        case 0x2A: // SLT
        NEXT_STATE.REGS[rd] = (rs_value < rt_value) ? 1 : 0;
        NEXT_STATE.PC += 4;
        printf("SLT\n");
        break;
        case 0x2B: // SLTU
        NEXT_STATE.REGS[rd] = (rs_value < rt_value) ? 1 : 0;
        NEXT_STATE.PC += 4;
        printf("SLTU\n");
        break;
        case 0x0C: //SYSCALL
        printf("SYSCALL\n");
            if (v0 == 0x0A) {  // Check if register $v0 has value 0x0A (decimal 10)
                NEXT_STATE.PC += 4;
                RUN_BIT = FALSE; // Stop simulation loop
        }
        break;
    }
    }
    else {
        uint32_t rs = (instruction >> 21) & 0x1F; // Extract source register
        uint32_t rt = (instruction >> 16) & 0x1F; // Extract destination register
        int16_t imm = instruction & 0xFFFF; // Extract signed immediate value
        if (imm & 0x8000) // If the sign bit is set
            imm |= 0xFFFF0000;
        uint32_t target = instruction & 0x03FFFFFF;
        uint32_t result;
        uint32_t address = CURRENT_STATE.REGS[rs] + imm;
        uint32_t byte = mem_read_32(address) & 0xFF;
        if (byte & 0x80) // If the sign bit is set
            byte |= 0xFFFFFF00;
        uint32_t halfword = mem_read_32(address) & 0xFFFF;
        if (halfword & 0x8000) 
            halfword |= 0xFFFF0000;
        uint32_t word = mem_read_32(address);
    
        switch(opcode31_26) { // Opcode from 31 to 26 bits
        case 0x02: // J
        target = (target << 2) | (CURRENT_STATE.PC & 0xF0000000); 
        NEXT_STATE.PC = target;
        printf("J\n");
        break;
        case 0x03: // JAL
        target = (target << 2) | (CURRENT_STATE.PC & 0xF0000000); 
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        NEXT_STATE.PC = target;
        printf("JAL\n");
        break;
        case 0x05: // BNE
        target = CURRENT_STATE.PC + 4 + (imm << 2);
        if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
            NEXT_STATE.PC = target;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BNE\n");
        break;
        case 0x06: // BLEZ
        target = CURRENT_STATE.PC + 4 + (imm << 2);
        if ((int32_t)CURRENT_STATE.REGS[rs] <= 0) {
            NEXT_STATE.PC = target;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BLEZ\n");
        break;
        case 0x07: // BGTZ
        target = CURRENT_STATE.PC + 4 + (imm << 2);
        if ((int32_t)CURRENT_STATE.REGS[rs] > 0) {
            NEXT_STATE.PC = target;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BGTZ\n");
        break;
        case 0x08: // ADDI
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
            NEXT_STATE.PC += 4;
            printf("ADDI\n");
        break;
        case 0x09: // ADDIU
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
            NEXT_STATE.PC += 4;
            printf("ADDIU\n");
        break;
        case 0x10: // BEQ
        target = CURRENT_STATE.PC + 4 + (imm << 2);
        if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
            NEXT_STATE.PC = target;
        } else {
            NEXT_STATE.PC += 4;
        }
        printf("BEQ\n");
        break;
        case 0x0A: // SLTI
        NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] < imm ? 1 : 0;
        NEXT_STATE.PC += 4;
        printf("SLTI\n");
        break;
        case 0x0B: // SLTIU
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < (uint32_t)imm ? 1 : 0;
        NEXT_STATE.PC += 4;
        printf("SLTIU\n");
        break;
        case 0x0C: // ANDI
             NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
             NEXT_STATE.PC += 4;
             printf("ANDI\n");
        break;
        case 0x0D: // ORI
        result = CURRENT_STATE.REGS[rs] | imm;
        NEXT_STATE.REGS[rt] = result;
        NEXT_STATE.PC += 4;
        printf("ORI\n");
        break;
        case 0x0E: // XORI
             NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ imm;
             NEXT_STATE.PC += 4;
             printf("XORI\n");
        break;
        case 0x0F: // LUI
            NEXT_STATE.REGS[rt] = imm << 16;
            NEXT_STATE.PC += 4;
            printf("LUI\n");
        break;
        case 0x20: // LB
        NEXT_STATE.REGS[rt] = byte;
        NEXT_STATE.PC += 4;
        printf("LB\n");
        break;
        case 0x21: // LH
        NEXT_STATE.REGS[rt] = halfword;
        NEXT_STATE.PC += 4;
        printf("LH\n");
        break;
        case 0x23: // LW
        NEXT_STATE.REGS[rt] = word;
        NEXT_STATE.PC += 4;
        printf("LW\n");
        break;
        case 0x24: // LBU
        NEXT_STATE.REGS[rt] = byte;
        NEXT_STATE.PC += 4;
        printf("LBU\n");
        break;
        case 0x25: // LHU
        NEXT_STATE.REGS[rt] = halfword;
        NEXT_STATE.PC += 4;
        printf("LHU\n");
        break;
        case 0x28: // SB
        byte = NEXT_STATE.REGS[rt] & 0xFF;
        mem_write_32(address, byte);
        NEXT_STATE.PC += 4;
        printf("SB\n");
        break;
        case 0x29: // SH
        halfword = NEXT_STATE.REGS[rt] & 0xFFFF;
        mem_write_32(address, halfword);
        NEXT_STATE.PC += 4;

        printf("SH\n");
        break;
        case 0x2B: // SW
        word = NEXT_STATE.REGS[rt];
        mem_write_32(address, word);
        NEXT_STATE.PC += 4;
        printf("SW\n");
        break;
    }
    }
}
