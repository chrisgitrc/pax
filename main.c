#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#define PAX_SUCCESS "[SUCCESS]"
#define PAX_ERROR	"[ERROR]"
#define PAX_FAILURE "[FAILURE]"
#define PAX_EXIT 255
#include "data.h"
#include "screen.h"
#include "./screens/ZyScreen.h"
#include "./screens/InfoScreen.h"

void PaxClose(Binary *binary);

#include "parser.h"
#include "dis.h"
#include "state.h"

#include "./init/init.h"

#define DEMO_FILE "/bin/ls"





Binary *PaxInit(char *f);
/*
* @param file file path to open
* @return Binary struct with elf, ph, and sh contents
*
*/
void PaxClose(Binary *b);
void print_color_background_pallete() {
	for (int i = 0; i < 256; ++i) {
		printf("%s%d", ALT_COLOR(i), i);
		}
	}
void print_color_foreground_pallete() {
	for(int i=0; i <256;++i) {
		printf("%s%d", ALT_COLOR_FOREGROUND(i),i);
		}
	}
void print_text_style_pallete() {
	}


#define USAGE "Usage: [-r | -e | -a | -d] [file]\n\t
-r [file]\tOpen machine instruction file. target architecture specified by --target. Open a new file if no [file] is specified.byte sequences are interpreted once after a set amount of time without input, and reinterpreted after more input is recieved. Interpretation is done as if in highest level of privilege.Interupts and MMIO are implemented as described in EXTERNAL section.\n\t
-e [file]\tOpen ELF file for emulation. emulation allows editing of instructions and instant reinterpretation. The emulation of system calls, interupts, and mmio can be configured to emulate a certain hardware/software scenario. System calls are initially set to passthrough to the native os if not pax file for syscalls is found. interupts and mmio disable the use of syscalls and are treated as nop. External 'device' configuration is described in the EXTERNAL section.\n\t
-a [file]\t open [file] or new file for assembly interpretation. --target specifies the assembly ISA. only .text and .data directives allowed.A fixed set of instructions are emulated/simulated and cpu state is saved after each fixed number of instruction are processed. modyfing a line reinterptus state from the closest saved snapshot.\n\t
-d debug a ELF file natively using ptrace. track memory allocations and stack state to display.\n
EXTERNAL\n\t 
Interupts and MMIO or syscalls are handled by setting up 'external' devices through a specified file per device, .pax files are interpreted from the calling directory and loaded into memory unless -d flag specified. A Pax file consists of two sections seperated by a single line with the section name surrouned by []. [DESC] and [CODE] are the sections. [DESC] describes the device including MMIO ranges, and if the device is a dedicated syscall handler. [CODE] describes the logic behind how the device triggers interupts or interacts with the emulated CPU. An empty or missing [CODE] leaves the user to manually trigger interupts in the IDE with the int NAME command to trigger interupt from device NAME . Successfully interpreted Pax files are executed on the native OS as seperate programs and connect to the emulated CPU in the main pax session through a socket found in /proc/pax/ if not specifed by -s. empty [CODE] files are handled in the main process"
int main(int argc, char *argv[]) {
	PInstance *ci = NULL;
	if (argc == 1) ci = NewPaxInstance(NULL)//default raw
	if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
		puts(USAGE);
	}
	else { 
		ci = NewPaxInstance(DEMO_FILE);
	}
	if (ci == NULL) return 1;
	Binary *elf = PaxInitElf(DEMO_FILE);
	if (elf ==NULL) {
		return -1;
		}
	char in = 0;


	PAX_ACTIVE_SCREEN active = DISM_SCREEN;
	SCREEN *screens[TOTAL_SCREENS];
	screens[MAIN_SCREEN]=ScreenInit(elf); //base screen metadata type settings always
	elf->buff = malloc(sizeof(uint8_t) * screens[MAIN_SCREEN]->height);
	elf->buffer = malloc(sizeof(ZydisDisassembledInstruction)*screens[MAIN_SCREEN]->height);
	printf("%s: Init Bounds Screen\n",PAX_SUCCESS);
	VM_CPU *cpu = InitCPU(elf); 
	printf("%s: Allocated Virtual Registers\n",PAX_SUCCESS);
	printf("Height = %u\n",screens[MAIN_SCREEN]->height);
	screens[DISM_SCREEN]=ScreenInitZydis(elf,cpu,screens[MAIN_SCREEN]->height,MAX_INSTRUCTION_MNEMONIC_LEN+17,screens[MAIN_SCREEN]->print_buffer,screens[MAIN_SCREEN]->length);
	STATE *stateM = malloc(sizeof(STATE));
	stateM->current=screens[active];
	stateM->StateMachineMode=StateMachineNormal;


	
	screens[DBG_SCREEN]=ScreenInitDBG(screens[MAIN_SCREEN]->print_buffer,91,1,20,30);
	screens[active]->handleinfo(screens[active]->trace,screens[DBG_SCREEN]->print_buffer,screens[DBG_SCREEN]->length, screens[DBG_SCREEN]->height);
	CLS();
	MOVE_ABSOLUTE(1,1);
	RenderUpdateScreen(screens[MAIN_SCREEN]->print_buffer,screens[MAIN_SCREEN]->height,screens[MAIN_SCREEN]->length);


	
	while (1) {
		if (PAX_EXIT == ci->smode(read(STDIN_FILENO, &in, 1), ci))
		if (read(STDIN_FILENO, &in, 1) & 1 &&
			PAX_EXIT == stateM->StateMachineMode(stateM,in)) {
			break;
		}
		switch (screens[active]->flags) {
			case  PAX_SCREEN_GO_UP:
			screens[active]->handleup(elf,screens[active], cpu);
			RenderUpdateScreen(screens[MAIN_SCREEN]->print_buffer,screens[MAIN_SCREEN]->height, screens[MAIN_SCREEN]->length);
			break;
			case PAX_SCREEN_GO_DN:
			
			screens[active]->handledn(elf,screens[active], cpu);
			RenderUpdateScreen(screens[MAIN_SCREEN]->print_buffer,screens[MAIN_SCREEN]->height, screens[MAIN_SCREEN]->length);

			break;
			case PAX_SCREEN_CHNGL:
			break;
			default:
			screens[active]->handleinfo(screens[active]->trace,screens[DBG_SCREEN]->print_buffer,screens[DBG_SCREEN]->length, screens[DBG_SCREEN]->height);
			//screens[DBG_SCREEN]->print_buffer[0][snprintf(screens[DBG_SCREEN]->print_buffer[0],10,"%b",screens[active]->flags)]=0x20;

			}
	}
	disable_rawio(&screens[MAIN_SCREEN]->trace->settings);
	CLS();
	PaxClose(elf);
	RESET_ATTRIB();
	EXIT_ALT_BUF();
	return 0;
}
Binary *PaxInit(char *file) {
	Binary *bin = malloc(sizeof(Binary));
	bin->buff = malloc(512);
	bin->fd = open(file, O_RDONLY);
	if (bin->fd < 0) {
		printf("%s: Could not open file '%s'", PAX_ERROR, file);
		PaxClose(bin);
		}
		
	memset(bin->buff, 0,512);
	if (NOT_VALID_ELF == PaxParseElfHdr(bin)) {
		PaxClose(bin);
		printf("%s:Not a valid ELF file. Exiting.",PAX_ERROR);
		return NULL;
		}
	if  (NOT_VALID_PHT == read_ph(bin)) {
		PaxClose(bin);
		printf("%s:Program header failure", PAX_FAILURE);
		return NULL;
		}
	free(bin->buff);
	return bin;
}
void PaxClose(Binary *b) {
	if	(b == NULL) return;
	if 	(b->buff != NULL) free(b->buff);
	if	(b->phtable != NULL) {
		free(b->phtable);
		}
	free(b);

	}
