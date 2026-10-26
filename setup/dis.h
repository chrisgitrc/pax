#include <Zydis/Zydis.h>
#include <inttypes.h>
#pragma once
#define MAX_INSTRUCTION_MNEMONIC_LEN 66
#define MAX_INSTRUCTION_LEN 15 //General Protection fault after 15 bytes

/*
*
*
* @param fd File Descriptor pointing to currently opened file by pax_open
* @param foff file offset into some loaded text section
* @param n how many instructions to decode generating n number of assembly lines
* @param PC address to start counting from
* @return A list containing ZydisDisassembledInstruction of size lines 
*/
void ZYDIS_DISASSEMBLE(uint8_t *buff, ZydisDisassembledInstruction *decoded_instructions, size_t n, ZyanU64 *PC) {
	ZyanUSize current_decoded=0;
	ZyanUSize byteoffset= 0;
	while (current_decoded < n && ZYAN_SUCCESS(ZydisDisassembleIntel(
		ZYDIS_MACHINE_MODE_LONG_64,
		*PC,
		buff+byteoffset,
		MAX_INSTRUCTION_LEN,
		&decoded_instructions[current_decoded]
		))) {
		byteoffset+=decoded_instructions[current_decoded].info.length;
		(*PC)+=decoded_instructions[current_decoded].info.length;
		++current_decoded;
		}
	}

void printDisassembledInstruction(const ZydisDisassembledInstruction* instr)
{
    printf("Runtime Address: 0x%016" PRIX64 "\n", instr->runtime_address);
    printf("Instruction Text: %s\n", instr->text);
    printf("Mnemonic: %s\n", ZydisMnemonicGetString(instr->info.mnemonic));
    printf("Length: %u bytes\n", instr->info.length);
    printf("Operand Count: %u\n", instr->info.operand_count);

    for (uint8_t i = 0; i < instr->info.operand_count; ++i)
    {
        const ZydisDecodedOperand* op = &instr->operands[i];
        printf("Operand %u:\n", i);
        printf("  Type: %d\n", op->type);
        printf("  Size: %u bits\n", op->size);
        printf("  Visibility: %d\n", op->visibility);
        printf("  Actions: %d\n", op->actions);

        switch (op->type)
        {
            case ZYDIS_OPERAND_TYPE_REGISTER:
                printf("  Register: %s\n", ZydisRegisterGetString(op->reg.value));
                break;

            case ZYDIS_OPERAND_TYPE_MEMORY:
                printf("  Segment: %s\n", ZydisRegisterGetString(op->mem.segment));
                printf("  Base: %s\n", ZydisRegisterGetString(op->mem.base));
                printf("  Index: %s\n", ZydisRegisterGetString(op->mem.index));
                printf("  Scale: %u\n", op->mem.scale);
                printf("  Displacement: 0x%" PRIX64 "\n", op->mem.disp.value);
                break;

            case ZYDIS_OPERAND_TYPE_POINTER:
                printf("  Segment: 0x%04X\n", op->ptr.segment);
                printf("  Offset: 0x%08X\n", op->ptr.offset);
                break;

            case ZYDIS_OPERAND_TYPE_IMMEDIATE:
                if (op->imm.is_signed)
                    printf("  Immediate (signed): %" PRId64 "\n", op->imm.value.s);
                else
                    printf("  Immediate (unsigned): %" PRIu64 "\n", op->imm.value.u);
                break;

            default:
                printf("  Unknown operand type\n");
                break;
        }
    }
}
