#ifndef _CPE_TIME_H_
#define _CPE_TIME_H_
//#include <asm/arch/moxa.h>
//#include <asm/arch/cpe_int.h>

#define TIMER1_COUNT                0x0
#define TIMER1_LOAD                 0x4
#define TIMER1_MATCH1               0x8
#define TIMER1_MATCH2               0xC
#define TIMER2_COUNT                0x10
#define TIMER2_LOAD                 0x14
#define TIMER2_MATCH1               0x18
#define TIMER2_MATCH2               0x1C
#define TIMER3_COUNT                0x20
#define TIMER3_LOAD                 0x24
#define TIMER3_MATCH1               0x28
#define TIMER3_MATCH2               0x2C 
#define TIMER_CR                    0x30
#define TIMER_INTR_STATE	0x34
#define TIMER_INTR_MASK		0x38

typedef struct
{       
	unsigned int Tm1En:1;		// Timer1 enable bit
	unsigned int Tm1Clock:1;	// Timer1 clock source (0: PCLK, 1: EXT1CLK)
	unsigned int Tm1OfEn:1;		// Timer1 over flow interrupt enable bit
	unsigned int Tm2En:1;
	unsigned int Tm2Clock:1;
	unsigned int Tm2OfEn:1;
	unsigned int Tm3En:1;
	unsigned int Tm3Clock:1;
	unsigned int Tm3OfEn:1;       
	unsigned int Reserved;        
} cpe_time_ctrl_t;

typedef struct 
{
	unsigned int TimerValue;
	unsigned int TimerLoad;
	unsigned int TimerMatch1;
	unsigned int TimerMatch2;
} cpe_time_reg_t;

extern cpe_time_reg_t *TimerBase[];

#if 0	// mask by Victor Yu. 01-31-2008
extern unsigned int cpe_timer_enable(unsigned int timer);
extern unsigned int cpe_timer_disable(unsigned int timer);
extern void cpe_timer_set_counter(unsigned int timer, unsigned int value);
extern void cpe_timer_set_reload(unsigned int timer, unsigned int value);
extern unsigned int cpe_timer_get_counter(unsigned int timer);
#endif

extern struct irqaction timer_irq;
extern unsigned long cpe_gettimeoffset(void);

#if 0	// mask by Victor Yu. 11-17-2005
static inline void cpe_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    do_timer(regs);
}

static inline void setup_timer(void)
{
#ifdef TIMER_INC_MODE
    cpe_timer_set_reload(1, 0xffffffff - APB_CLK/HZ);
    cpe_timer_set_counter(1, 0xffffffff - APB_CLK/HZ);
#else
    cpe_timer_set_reload(1, APB_CLK/HZ);
    cpe_timer_set_counter(1, APB_CLK/HZ);
#endif

    if(!cpe_timer_enable(1))
    {
		panic("can not enable timer\n");
	}
	
	printk("IRQ timer at interrupt number 0x%x clock %d\r\n",IRQ_TIMER1,APB_CLK);
	cpe_int_set_irq(IRQ_TIMER1, EDGE,L_ACTIVE);
	timer_irq.handler = cpe_timer_interrupt;
	timer_irq.flags = IRQF_DISABLED | IRQF_TIMER; //ivan added
	setup_arm_irq(IRQ_TIMER1, &timer_irq);
}
#endif

#endif	// _CPE_TIME_H
