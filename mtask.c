#include "bootpack.h"

struct TIMER *mt_timer;
int mt_tr;//����tr�Ĵ���

void mt_init(void)
{
	mt_timer = timer_alloc();
	//û�б�Ҫtimer_init����ʱʱ����Ҫ��fifo��д����
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