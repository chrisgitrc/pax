#include "screenact.h"
int actonkey(SCREEN *from, PInstance *cxt) {
	char in;
	read(STDIN_FILENO, &in, 1);
	switch (from->keymap[in]) {
		case NOMAP:
			return PAX_NOP;
		case UP:
		cxt->screen
		break;
		case DOWN:
		break;
		case LEFT:
		break;
		case RIGHT:
		break;
		case FRIGHT:
		break;
		case FLEFT:
		break;
		case FDOWN:
		break;
		case FUP:
		break;
		case CLOSE:
		break;
	}
}
