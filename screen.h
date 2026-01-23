#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "dis.h"

#define PHFLG_EXEC 0x1
#define PHFLG_WRTE 0x2
#define PHFLG_READ 0x4

#define ALT_ENTER "\x2b[?1049h"
#define ALT_EXIT "\x1b[?1049l"
#define ALT_CLS "\x1b[2J"
#define ALT_COLOR_RESET "\x1b[0m"

#define ALT_HIDE "\x1b[?25l"
#define ALT_SHOW "\x1b[?25h"
#define ALT_SAVE "\x1b[s"
#define ALT_RES "\x1b[u"

#define PAX_SCREEN_GO_UP 0x1	
#define PAX_SCREEN_GO_DN 0x2
#define PAX_SCREEN_CHNGL 0x4

#define MAX_SCREEN_PAGES 3

/*
	*0-9 text style
	*10-19 font select
	*20-29 text style reset
	*30-37 foreground;
	*40-47 background;
	*90-97 foreground;
	*100-107 background;
*/

void CURSOR_SAVE();
void CURSOR_LOAD();
void DEBUG_PRINT(const char *str) {
	static uint8_t pos = 1;
	CURSOR_SAVE();
	printf("\x1b[%u;80H",pos);
	printf("%s",str);
	CURSOR_LOAD();
	++pos;
	if (pos> 30)pos=0;
	return;
	}
void ENTER_ACS() {
	printf("\x1b(0");
	}
void EXIT_ACS() {
	printf("\x1b(B");
	}
void CLS() {
	puts(ALT_CLS);
	}
void RESET_ATTRIB() {
	printf("\x1b[0m");
	}
void ENTER_ALT_BUF() {
	puts(ALT_ENTER);
	}
void EXIT_ALT_BUF() {
	puts(ALT_EXIT);
	}
void DIMENSIONS_BUF(unsigned short *cols, unsigned short *rows) {

  struct winsize w;
    int tty_fd = open("/dev/tty", O_RDWR);
    if (tty_fd == -1) {
        perror("open /dev/tty");
        return;
    }

    if (ioctl(tty_fd, TIOCGWINSZ, &w) == -1) {
        perror("ioctl TIOCGWINSZ");
        close(tty_fd);
        return;
    }

    printf("Rows: %d, Columns: %d\n", w.ws_row, w.ws_col);
    *rows = w.ws_row;
    *cols = w.ws_col;
    close(tty_fd);

}
void ALT_BUF_MOVE(unsigned short col, unsigned short row, SCREEN *screen) {
//	printf("\x1b[s\x1b[5;1H want to move to %s %s\x1b[u", col, row);
	if (col < screen->lbound_x ) {
		screen->x = screen->lbound_x;
		}
	else if (col > screen->rbound_x) {
		screen->x = screen->rbound_x;	
		}
	else if (row < screen->rbound_y+1) {
		screen->y = screen->rbound_y+1;
		screen->flags |= PAX_SCREEN_GO_UP;
		}
	else if (row > screen->lbound_y) {

		screen->y=screen->lbound_y;
		screen->flags |= PAX_SCREEN_GO_DN;
		}
	printf("\x1b[%d;%dH", row, col);
	}
	
void ALT_RESET() {
	puts(ALT_SHOW);
	}
char *ALT_COLOR(unsigned char color) {
	printf("\x1b[48;5;%um", color);
	return "";
	}
char *ALT_COLOR_FOREGROUND(unsigned char color){
	printf("\x1b[38;5;%um",color);
	return "";
	}
char *ALT_STYLE(enum ALT_TEXT_STYLE code){
	printf("\x1b[%um",code);
	return "";
	}
char* ALT_RESET_COLOR(){
	printf("\x1b[0m");
	return "";
	}
void CURSOR_SAVE() {
	printf("%s", ALT_SAVE);

	}
void CURSOR_LOAD() {
	printf("%s", ALT_RES);
	}
struct termios orig_termios;
void disable_rawio(struct termios *og) {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, og);
	}
void enable_rawio(struct termios *base) {

	struct termios raw;
	tcgetattr(STDIN_FILENO, base);
	raw = *base;
	raw.c_lflag &= ~(ECHO | ICANON );

	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	setbuf(stdout, NULL);

	}
void CLEAR_LINE_N(uint16_t line ){
	printf("\x1b[s\x1b[%u;1H\x1b[2k\x1b[u", line);
	}
void MOVE_ABSOLUTE(uint32_t x, uint32_t y) {
	printf("\x1b[%u;%uH",y,x);
}
size_t RenderUpdateScreen(char **print_buffer, uint32_t height,uint32_t length) {
	CURSOR_SAVE();
	for (int i = 0; i <height;++i) {
			printf("%s\n",print_buffer[i]);
			memset(print_buffer[i]);
		}
	CURSOR_LOAD();
}
	
