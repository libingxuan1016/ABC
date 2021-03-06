// asmhead.nas
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; 
	char leds; 
	char vmode; 
	char reserve;
	short scrnx, scrny;
	char *vram;
};

#define ADR_BOOTINFO	0x00000ff0

//naskfunc.nas
//C语言实现不了的函数 用汇编实现
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void taskswitch4(void);
void taskswitch3(void);
void farjmp(int eip, int cs);

//fifo.c
struct FIFO32 {
	int *buf; //FIFO8所在地址指针
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

//graphic.c
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);
	
#define COL8_000000		0	/*  0:黑 */
#define COL8_FF0000		1	/*  1:亮红 */
#define COL8_00FF00		2	/*  2:亮緑 */
#define COL8_FFFF00		3	/*  3:亮黄 */
#define COL8_0000FF		4	/*  4:亮蓝 */
#define COL8_FF00FF		5	/*  5:亮紫 */
#define COL8_00FFFF		6	/*  6:浅亮蓝 */
#define COL8_FFFFFF		7	/*  7:白 */
#define COL8_C6C6C6		8	/*  8:亮灰 */
#define COL8_840000		9	/*  9:暗红 */
#define COL8_008400		10	/* 10:暗緑 */
#define COL8_848400		11	/* 11:暗黄 */
#define COL8_000084		12	/* 12:暗青 */
#define COL8_840084		13	/* 13:暗紫 */
#define COL8_008484		14	/* 14:浅暗蓝 */
#define COL8_848484		15	/* 15:暗灰 */

// dsctbl.c
struct SEGMENT_DESCRIPTOR {	// GDT的8字节内容
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR { //IDT的8字节内容
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e
#define AR_TSS32		0x0089

//int.c
void init_pic(void);
void inthandler2c(int *esp);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

//keyboard.c
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo, int data0);
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

//mouse.c
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void inthandler2c(int *esp);
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

//memory.c
#define MEMMAN_FREES 4090 //大概可以管理32KB
#define MEMMAN_ADDR 0x003c0000

struct FREEINFO { //可用信息
	unsigned int addr, size;
};

struct MEMMAN { //内存管理
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

//sheet.c
#define MAX_SHEETS 256
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
};
struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;//最上面图层的高度
	struct SHEET *sheets[MAX_SHEETS];//按图层顺序
	struct SHEET sheets0[MAX_SHEETS];//按地址顺序
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

//timer.c
#define MAX_TIMER 500
struct TIMER {
	struct TIMER *next;
	unsigned int timeout, flags;///timeout:距离超时还有多长时间
	struct FIFO32 *fifo;//timeout=0，就往fifo缓冲区里发送数据
	int data;
};
struct TIMERCTL {
	unsigned int count, next, using;//using 相当于shtctl里面的top
	struct TIMER *t0;//队列的第一个
	struct TIMER timers0[MAX_TIMER];//按照地址
};
extern struct TIMERCTL timerctl;
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);

/* mtask.c */
#define MAX_TASKS 1000//最大任务数
#define TASK_GDT0 3//定义从GDT的几号开始分配给GSS
#define MAX_TASKS_LV	100 //每个level最多100个任务
#define MAX_TASKLEVELS	10//一共10个level
struct TSS32 {//任务状态段 task status segment
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK {
	int sel, flags; /* sel是GDT的编号 */
	struct TSS32 tss;
	struct FIFO32 fifo;
	int level, priority;//level、优先级
};

struct TASKLEVEL {
	int running;//正在运行的任务数量
	int now;//当前正在运行的任务
	struct TASK *tasks[MAX_TASKS_LV];
};

struct TASKCTL {
	int now_lv;//现在活动中的level
	int lv_change;//在下次活动中是否需要改level
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

extern struct TIMER *task_timer;
struct TASK *task_now(void);
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);
