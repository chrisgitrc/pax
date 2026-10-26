#pragma once
#include <Zydis/Zydis.h>
#include <inttypes.h>
#include <termios.h>
#define MAX_SCREEN_PAGES 3
/*	
	On pax_open() create a new Debug object.
	it saves debug information from readelf and the parsed result
	creates length and alignment to load and store binary .data, .rodata, .bss to save memory.
	call zydis on each page pulled and save to memory
	page size = 512 , 1024, 4069 using mmap
*/
enum ALT_TEXT_STYLE {
	TEXT_STYLE_BOLD=1,
	TEXT_STYLE_FAINT,
	TEXT_STYLE_ITALIC,
	TEXT_STYLE_UNDERLINE,
	TEXT_STYLE_BLINK,
	TEXT_STYLE_REVERSE,
	TEXT_STYLE_HIDDEN,
	TEXT_STYLE_STRIKETHROUGH,

	};
typedef enum PAX_RETURN_STATUS{
	PAX_OKAY,
	NOT_VALID_ELF,
	NOT_VALID_PHT,
	NOT_VALID_SHT,

	}PAX_RETURN_STATUS;
typedef enum PAX_ACTIVE_SCREEN{
	MAIN_SCREEN,
	DISM_SCREEN,
	STACK_SCREEN,
	HEAP_SCREEN,
	DBG_SCREEN,
	TOTAL_SCREENS
	}PAX_ACTIVE_SCREEN;
typedef enum INPUT_STATUS{
	STATE_INPUT,
	STATE_NORMAL,
	STATE_COMMAND,
	}STATUS;
typedef enum BUFFER_POS{
	POS_BEFORE,
	POS_HERE,
	POS_AFTER
	}BUFFER_POS;
typedef struct PHENTRY {
	uint32_t type;	
	uint32_t flags;
	uint64_t offset;
	uint64_t vaddr;
	uint64_t paddr;
	uint64_t filesz;
	uint64_t memsz;
	uint64_t align;
	}PHENTRY;

typedef struct Elf_header64{
	uint32_t Magic; 
	uint8_t Width;
	uint8_t Data;
	uint8_t eVersion;
	uint8_t ABI;
	uint64_t Padding;
	uint16_t Type;
	uint16_t Arch;
	uint32_t Version;
	uint64_t Entry;
	uint32_t Flags;
	uint16_t ESize;
	uint64_t PHEntry;
	uint16_t PHESize;
	uint16_t PHENum;
	uint64_t SHEntry;
	uint16_t SHENSize;
	uint16_t SHENum;
	uint8_t SHSTRNDX;
	}ELF_HEADER;
typedef struct CPU_STATE{
	uint64_t rax;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbx;
	uint64_t rsp;
	uint64_t rbp;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rip;
	}VM_CPU;
typedef struct binfile{ 
	uint8_t *buff; // = height * MAX_INSTRUCTION_LEN 
	int fd;
	size_t read; //for read();
	ELF_HEADER e_hdr;
	PHENTRY *phtable;
	short xseg;
//	SHTABLE *shtable;
	uint16_t max_instruction_pages;
	ZydisDisassembledInstruction *buffer; // = height *zu(ZDI) used by zydis disassembled screen
	
	}Binary;
typedef struct mmapVAR{
	char *name; //symbol name if found else alphbetical
	uint64_t addr; //virtual
	uint16_t refs; //reference count
	}Variable;
typedef struct MEMSTACK {
	uint64_t address;
	uint8_t value;
	}MEMSTACK;
typedef struct PROCESS_HEAP {
	uint64_t address;
	uint8_t value;
	}HEAP;
typedef enum DISPLAY_TYPE{
	DISPLAY_TYPE_MAIN,
	DISPLAY_TYPE_INSTRUCTIONS,
	DISPLAY_TYPE_PSTACK,
	DISPLAY_TYPE_PHEAP,
	DISPLAY_TYPE_SETTINGS,
	DISPLAY_TYPE_INSTRUCTION_INFO
	}DISPLAY_TYPE;
typedef struct ZydisScreenBuffer{
	uint64_t foff;  //filled by LOAD_ZYSC
	uint32_t self_i; // your position 0-end of text sextion` filled by caller of LOAD_ZYSC
	VM_CPU context; // caching cpu state for live ee filled by LOAD_ZYSC
	uint8_t **bytes;  //  machine code bytes //filled by LOAD_ZYSC
	char **text; //filled by LOAD_ZYSC
	ZydisDisassembledInstruction * current; // if not current buffer, null used for printing instruction info in instruction info screen
	}ZydisScreen;
typedef struct DEBUG_SCREEN{
	char **lines;
	uint32_t total_lines;
	}DBG_S;

//specific meta to type
typedef struct DETAILED_INFO_ON_CURRENT_SELECTION{}DetailsScreen;
typedef union SCREEN_DISPLAY_TYPE{
	struct termios settings;
	ZydisScreen* zysc[3];
	MEMSTACK *pstack;
	HEAP *pheap;
	DBG_S *DBG; 
	DetailsScreen *meta;
	}PROCTRACE;
/*
screen type each is its own container with its own fd for proximity caching
types are of stack and heap memory, zydisdisassembledinstruction*, and UI
*/
struct file_buffer;
typedef struct file_buffer{
	int fd;
	unsigned x, y; //never 0 start at 1;1 top left mov(y;x)
	uint32_t page_y;
	unsigned short height, length; //current dimensions
	unsigned lbound_x, rbound_x;
	unsigned lbound_y, rbound_y; //bottom top
	DISPLAY_TYPE display_type;
	unsigned display_length;
	PROCTRACE *trace; 
	uint8_t flags;
	//needed functions for all type of screens dependent to SCREEN.trace specific data
	void (*handleup)(Binary*, struct file_buffer*, VM_CPU*);
	void (*handledn)(Binary *, struct file_buffer*,VM_CPU*);
	void (*handleinfo)(PROCTRACE *, char **, uint32_t length, uint32_t height);

	uint64_t *foffsets;
	char **print_buffer; //= (x*y)+\1
	}SCREEN;
typedef struct state_machine{
	SCREEN *current;
	uint8_t (*StateMachineMode)(struct state_machine *,char);
	char input_buffer[4096];
	}STATE;
typedef struct DebugObject {
	
	short alignment;
	char *data;
	char *rodata;
	char *bss;
	Variable *variables;
		
}Object;

