#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
	char li[40][100];
	memset(li[5],'a',50);
	char list[40*100+1];
	memset(list,'a',50);
	memset(list,0x20,30);
	while (1) {
		printf("%s",list);
	}
	return 0;
}
