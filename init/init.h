COMPUTER *initPC(Binary *b) {
	switch (b->e_hdr.Arch) {
		case EM_X86_64:
						
		break;
		case EM_ARM:
		break;
		case EM_ARM64:
		break;
		default:
			return PAX_UNSUPPORTED_ARCHITECTURE;
	}
	COMPUTER *pc = malloc(sizeof (struct COMPUTER));
	pc->cpu = initCPU();
	pc->mem = initMEM();
	return COMPUTER;
}
VM_CPU *InitCPU(Binary *b) {
	VM_CPU * cpu = malloc(sizeof(VM_CPU));
	cpu->rip = b->phtable[b->xseg].vaddr;
	}
SCREEN *ScreenInit(Binary *b) {
	CLS();
	ENTER_ALT_BUF();
	SCREEN *s = malloc(sizeof(SCREEN));
	s->trace = malloc(sizeof(struct termios));
	CLS();enable_rawio(&s->trace->settings);CLS();
	s->x=1;
	s->y=1;
	DIMENSIONS_BUF(&s->length,&s->height);
	s->page_y=0;
	s->print_buffer = malloc(sizeof(char*);
	*(s->print_buffer) = malloc(s->length*s->height+1);
	}
	CLEAR_LINE_N(1);
	s->handleup=NULL;
	return s;
	
	
}
uint32_t InitZYSC(Binary *b, ZydisScreen ** zysc, uint64_t file_entry, uint64_t *foffsets, uint16_t height) {
	uint8_t *data = malloc(height * MAX_INSTRUCTION_LEN +MAX_INSTRUCTION_LEN);
	ZydisDisassembledInstruction *instruction = b->buffer;
	uint64_t pc = b->phtable[b->xseg].vaddr;
	uint64_t section_length = 0, total_bytes_read = 0;
	uint64_t xseg_foff = b->phtable[b->xseg].offset;
	uint64_t xseg_sz = b->phtable[b->xseg].filesz;
	uint32_t max_instruction_pages;
	file_entry=0x3000;
	for (uint64_t i = 0;;++i) {
		lseek(b->fd,file_entry,SEEK_SET);
		foffsets[i] = file_entry;
		section_length = 0;
		printf("foffsets[%lu]: 0x%lx\n",i , file_entry);
		b->read = read(b->fd, data,height * MAX_INSTRUCTION_LEN+ MAX_INSTRUCTION_LEN);
		printf("read: %lu\n", b->read);
		ZYDIS_DISASSEMBLE(data,instruction,height,&pc);
		for (uint32_t j = 0; j < height;++j) {
			section_length +=instruction[j].info.length;
			printf("%s\n",instruction[j].text);


			//overflow
			if (file_entry + section_length >xseg_sz+xseg_foff)  {
				section_length-=instruction[j].info.length;
				uint8_t *bytes = &data[section_length];
				printf("%016"PRIX64":fixing instruction overflow bytes:\n", file_entry + section_length);
				for (int i = 0; i<instruction[j].info.length;++i)printf("\\x%x",bytes[i]);
				ZydisDisassembleIntel(
						ZYDIS_MACHINE_MODE_LONG_64,
						pc,
						bytes,
						file_entry+section_length+instruction[j].info.length-xseg_sz-xseg_foff,
						instruction
						);
				printf("\nNew instruction:\n");
				for (int i = 0; i<section_length+instruction[j].info.length-section_length;++i)printf("\\x%x",bytes[i]);
				puts("");


				free(data);
				return max_instruction_pages;
				}
			}
		file_entry += section_length;
		++max_instruction_pages;

		}
	return max_instruction_pages;
	 
	free(data);
}

SCREEN *ScreenInitZydis(Binary *b, VM_CPU *cpu, uint16_t height,uint16_t length, char **main_print, uint16_t screen_length) {
	SCREEN *s = malloc(sizeof(SCREEN));
	printf("%s:sizeof(SCREEN)\n", PAX_SUCCESS);
	s->handleup=SCREENINSHANDLEUP;
	s->handledn=SCREENINSHANDLEDN;
	s->handleinfo=SCREENINSHANDLEINFO;
	s->x=1;
	s->y=1;
	s->length=length;
	s->height=height;
	s->lbound_x=0; //exclusive of 0
	if (MAX_INSTRUCTION_MNEMONIC_LEN +17>screen_length){
		printf("dis screen too big");
		return NULL;
		}
	s->rbound_x=MAX_INSTRUCTION_MNEMONIC_LEN + 17;
	s->lbound_y=height;
	s->rbound_y=0;
	s->page_y = 0;
	s->print_buffer = malloc(sizeof(char*)*s->height);
	for (int i = 0;i<s->height;++i) {
		s->print_buffer[i] = main_print[i] + s->lbound_x;
		}
	s->print_buffer = main_print+s->rbound_y;

	s->trace = malloc(sizeof(PROCTRACE));
	s->trace->zysc[POS_HERE] = malloc(sizeof(ZydisScreen));
	s->trace->zysc[POS_BEFORE] = malloc(sizeof(ZydisScreen));
	s->trace->zysc[POS_AFTER] = malloc(sizeof(ZydisScreen));
	printf("%s:sizeof(ZydisScreen*)*3\n", PAX_SUCCESS);
	s->foffsets = malloc(sizeof(uint64_t)*((b->phtable[b->xseg].filesz/s->height)+1));
	printf("%s:sizeof(uint64_t)*((b->phtable[b->xseg].filesz/s->height)+1)\n", PAX_SUCCESS);
	b->max_instruction_pages =InitZYSC(b, s->trace->zysc, b->phtable[b->xseg].offset, s->foffsets, s->height);
	
	s->trace->zysc[POS_HERE]->bytes = malloc(sizeof(uint8_t *)*s->height);
	s->trace->zysc[POS_AFTER]->bytes = malloc(sizeof(uint8_t*)*s->height);
	s->trace->zysc[POS_BEFORE]->bytes =malloc(sizeof(uint8_t*)*s->height);
	s->trace->zysc[POS_BEFORE]->text= malloc(sizeof(char* )* s->height);
	s->trace->zysc[POS_HERE]->text= malloc(sizeof(char *)* s->height);
	s->trace->zysc[POS_AFTER]->text= malloc(sizeof(char *)* s->height);
	for (int i = 0; i< s->height;++i) {
	s->trace->zysc[POS_BEFORE]->bytes[i] = malloc(sizeof(uint8_t) * MAX_INSTRUCTION_LEN);
	s->trace->zysc[POS_HERE]->bytes[i] = malloc(sizeof(uint8_t) * MAX_INSTRUCTION_LEN);
	s->trace->zysc[POS_AFTER]->bytes[i] = malloc(sizeof(uint8_t) * MAX_INSTRUCTION_LEN);
	s->trace->zysc[POS_BEFORE]->text[i] = malloc(sizeof(char) * MAX_INSTRUCTION_MNEMONIC_LEN);
	s->trace->zysc[POS_HERE]->text[i] = malloc(sizeof(char) * MAX_INSTRUCTION_MNEMONIC_LEN);
	s->trace->zysc[POS_AFTER]->text[i] = malloc(sizeof(char) * MAX_INSTRUCTION_MNEMONIC_LEN);
		}
	s->display_type = DISPLAY_TYPE_INSTRUCTIONS;
	s->display_length=40;

	s->trace->zysc[POS_HERE]->current = malloc(sizeof(ZydisDisassembledInstruction));
	s->trace->zysc[POS_BEFORE]->current = malloc(sizeof(ZydisDisassembledInstruction));
	s->trace->zysc[POS_AFTER]->current = malloc(sizeof(ZydisDisassembledInstruction));
	s->trace->zysc[POS_BEFORE]->foff = s->foffsets[0];
	s->trace->zysc[POS_BEFORE]->self_i = 0;
	s->trace->zysc[POS_HERE]->foff = s->foffsets[0];
	s->trace->zysc[POS_HERE]->self_i = 0;
	s->trace->zysc[POS_AFTER]->foff = s->foffsets[1];
	s->trace->zysc[POS_AFTER]->self_i = 1;
	Load_ZYSC(b, s,POS_HERE, &cpu->rip, s->foffsets[0]);
	printf("%s: Loaded Entry Point DISM\n", PAX_SUCCESS);
	Load_ZYSC(b,s,POS_AFTER, &cpu->rip,s->foffsets[1]);
	printf("%s: Loaded Entry Point DISM + 1\n", PAX_SUCCESS);
	Load_ZYSC(b, s,POS_BEFORE, &cpu->rip, s->foffsets[0]);
    for (int i = 0; i < s->height;++i ) {
		memcpy(main_print[i],s->trace->zysc[POS_HERE]->text[i],strlen(s->trace->zysc[POS_HERE]->text[i]));
	}
	return s;
	}
SCREEN *ScreenInitDBG(char **screen,unsigned x,unsigned y,unsigned length,unsigned height) {
	SCREEN *s = malloc(sizeof(SCREEN));
	s->x=1;
	s->y=1;
	s->lbound_x=x;
	s->rbound_x=x+length;
	s->length = s->lbound_x-s->rbound_x;
	s->rbound_y=y;
	s->lbound_y=y+height;
	s->height = s->lbound_y-s->rbound_y;
	s->display_type=DBG_SCREEN;	
	s->trace = malloc(sizeof(PROCTRACE));
	s->trace->DBG=malloc(sizeof(DBG_S));
	s->trace->DBG->lines = malloc(sizeof(char*)*height);
	s->trace->DBG->total_lines =height;
	s->print_buffer = malloc(sizeof(char*)*height);
	for(int i =0; i<height;++i) {
		s->trace->DBG->lines[i] = malloc(length);
		s->print_buffer[i] = screen[i] + s->lbound_x;
		}

		
	return s;
	}
