void Load_ZYSC(Binary *b, SCREEN *dism_s, BUFFER_POS pos, uint64_t *PC, uint64_t foff){

//TODO set context on load
	uint8_t **bytes = dism_s->trace->zysc[pos]->bytes;
	char **text = dism_s->trace->zysc[pos]->text;
	PC = (PC==NULL)?&dism_s->trace->zysc[pos]->context.rip:PC;
	uint64_t lpc = 0;
	ZydisDisassembledInstruction *instruction = b->buffer;
	lseek(b->fd,foff,SEEK_SET);
	b->read = read(b->fd, b->buff, MAX_INSTRUCTION_LEN*dism_s->height);
	ZYDIS_DISASSEMBLE(b->buff,b->buffer,dism_s->height,PC);


	for (int i = 0; i< dism_s->height; ++i) {
		for(uint8_t j = 0; j < b->buffer[i].info.length;++j) {
			bytes[i][j] = b->buff[lpc+j];
		} 
		memset(text[i],'\0',dism_s->length);
		memcpy(text[i],instruction[i].text,sizeof(instruction[i].text));


		lpc +=b->buffer[i].info.length;
	}

}

void SCREENINSHANDLEUP(Binary *b, SCREEN *s, VM_CPU *cpu){
		s->flags ^= PAX_SCREEN_GO_UP;
	if (s->page_y == 0) { 
		if (s->trace->zysc[POS_HERE]->self_i == 0) {
			return;
			}
		
		*(s->trace->zysc[POS_AFTER]) = *(s->trace->zysc[POS_HERE]);
		*(s->trace->zysc[POS_HERE]) = *(s->trace->zysc[POS_BEFORE]);
		s->trace->zysc[POS_BEFORE]->self_i = s->trace->zysc[POS_AFTER]->self_i-1;
		Load_ZYSC(b, s,POS_BEFORE, &cpu->rip, s->foffsets[s->trace->zysc[POS_BEFORE]->self_i]);
		s->trace->zysc[POS_HERE]->current = b->buffer+(s->height -1);
		s->trace->zysc[POS_BEFORE]->foff = s->foffsets[s->trace->zysc[POS_BEFORE]->self_i];
		
		s->page_y =s->height;
	}
		--s->page_y;
	//inbetween two pages with cursor on PAGE_HERE going up with PAGE_AFTER still viewable
	//merge text sections to make up one frame render
	//print each segment individually
		char **text = s->trace->zysc[POS_HERE]->text+s->page_y; //first half of segment
		uint32_t ln=0;
		

		for (int i = 0; i < s->height-s->page_y;++i,++ln) {
		memset(s->print_buffer[ln],0x20,s->length);
		memcpy(s->print_buffer[ln],text[i],strlen(text[i]));
		}
		text = s->trace->zysc[POS_AFTER]->text;
		for (int i=0;i<s->page_y;++i,++ln) {
		memset(s->print_buffer[ln],0x20,s->length);
		memcpy(s->print_buffer[ln],text[i],strlen(text[i]));
		}
		s->trace->zysc[POS_HERE]->current = &b->buffer[s->page_y];
}
void SCREENINSHANDLEDN(Binary *b, SCREEN *s, VM_CPU *cpu){
	s->flags ^= PAX_SCREEN_GO_DN;
	if (s->trace->zysc[POS_HERE]->self_i == b->max_instruction_pages-1) {
		return;
	}
	if (s->page_y==s->height-1){
	*(s->trace->zysc[POS_BEFORE]) = *(s->trace->zysc[POS_HERE]);
	*(s->trace->zysc[POS_HERE]) = *(s->trace->zysc[POS_AFTER]);
s->trace->zysc[POS_AFTER]->self_i = s->trace->zysc[POS_HERE]->self_i+1;
	Load_ZYSC(b, s,POS_AFTER, &cpu->rip, s->foffsets[s->trace->zysc[POS_HERE]->self_i]);
	s->trace->zysc[POS_AFTER]->foff = s->foffsets[s->trace->zysc[POS_AFTER]->self_i];
	s->page_y=0;
	}
	uint16_t ln = 0;
	int fdf=open("pax.log", O_CREAT | O_APPEND | O_WRONLY);
	int og = dup(STDOUT_FILENO);
	dup2(fdf,STDOUT_FILENO);
	for (int i=s->page_y; i < s->height;++i,++ln) {
		printf("%d ", ln);
		memset(s->print_buffer[ln],0x20,s->length);
		memcpy(s->print_buffer[ln],s->trace->zysc[POS_BEFORE]->text[i],strlen(s->trace->zysc[POS_BEFORE]->text[i]));
		}
	printf("second ");
	for (int i=0; i < s->page_y;++i,++ln) {
		printf("%"PRIu16" ", ln);
		memset(s->print_buffer[ln],0x20,s->length);
		memcpy(s->print_buffer[ln],s->trace->zysc[POS_HERE]->text[i],strlen(s->trace->zysc[POS_HERE]->text[i]));
		}
		puts("");
	close(fdf);
	dup2(og,STDOUT_FILENO);
	
	
	s->trace->zysc[POS_HERE]->current=&b->buffer[s->page_y];
	++s->page_y;
}
void SCREENINSHANDLEINFO(PROCTRACE *m, char **print_buffer, uint32_t length, uint32_t height) {
	ZydisScreen *meta = m->zysc[POS_HERE];
	
	if (height < 5){
		return;
		}
	print_buffer[0][snprintf(print_buffer[0],length, "%"PRIX64,meta->current->runtime_address)] = 0x20;	
	print_buffer[1][snprintf(print_buffer[1],length, "%s",meta->current->text)]=0x20;	

	print_buffer[2][snprintf(print_buffer[2],length, "Reg: %s",ZydisRegisterGetString(meta->current->operands[0].reg.value))] = 0x20;	

	}
