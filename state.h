uint8_t StateMachineInsert(STATE *s,char in);
uint8_t StateMachineCommand(STATE *s,char in);
uint8_t StateMachineVisual(STATE *s,char in);
uint8_t StateMachineNormal(STATE *s,char in) {
	switch (in) {
		case ':':
			s->StateMachineMode=StateMachineCommand; 
			break;
		case 'i':
			s->StateMachineMode=StateMachineInsert;	
			break;
		case 'v':
			s->StateMachineMode=StateMachineVisual;	
			break;
		case 'j':
			ALT_BUF_MOVE(s->current->x, ++s->current->y,s->current);
			break;
		case 'k':
			ALT_BUF_MOVE(s->current->x, --s->current->y,s->current);
			break;
		case 'h':
			ALT_BUF_MOVE(--s->current->x, s->current->y,s->current);
			break;
		case 'l':
			ALT_BUF_MOVE(++s->current->x, s->current->y,s->current);
			break;
		case 'Q':
		return PAX_EXIT;
	}	
}

uint8_t StateMachineInsert(STATE *s,char in) {
	}
uint8_t StateMachineCommand(STATE *s,char in) {
	}
uint8_t StateMachineVisual(STATE *s,char in) {
	}
