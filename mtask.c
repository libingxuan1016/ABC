#include "bootpack.h"

struct TIMER *mt_timer;
int mt_tr;//代表tr寄存器

void mt_init(void)
{
	mt_timer = timer_alloc();
	//没有必要timer_init：超时时不需要往fifo里写数据
	timer_settime(mt_timer, 2);
	mt_tr = 3 * 8;
	return;
}

void mt_taskswitch(void)
{
	if (mt_tr == 3 * 8) {
		mt_tr = 4 * 8;
	} else {
		mt_tr = 3 * 8;
	}
	timer_settime(mt_timer, 2);
	farjmp(0, mt_tr);
	return;
}
