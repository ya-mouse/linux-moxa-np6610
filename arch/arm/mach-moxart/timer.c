/*
 * time.c  Timer functions for cpe
 */
//#include <linux/config.h>
#include <mach/moxa.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <mach/cpe_int.h>
#include <mach/cpe_time.h>
#include <asm/mach/time.h>

#define MAX_TIMER	2
#define USED_TIMER	1
#define SET_COUNTER	(APB_CLK / HZ)

cpe_time_reg_t *TimerBase[] =
{
	0, 
	(cpe_time_reg_t *)CPE_TIMER1_VA_BASE,
	(cpe_time_reg_t *)CPE_TIMER2_VA_BASE
};

static int cpe_timer_enable(int timer)
{   
	cpe_time_ctrl_t *t=(cpe_time_ctrl_t *)(CPE_TIMER1_VA_BASE+TIMER_CR);

	if ((timer == 0) || (timer > MAX_TIMER))
		return 0;

	switch(timer)
	{
        case 1:
    	    t->Tm1En=1;
    	    t->Tm1OfEn=1;
#if 1	// add by Victor Yu. 06-01-2005
	    t->Tm1Clock = 0;	// use PCLK
#endif
        	break;    
        case 2:
    	    t->Tm2En=1;
    	    t->Tm2OfEn=1;
#if 1	// add by Victor Yu. 06-01-2005
	    t->Tm2Clock = 0;	// use PCLK
#endif
        	break; 
        case 3:
    	    t->Tm3En=1;
    	    t->Tm3OfEn=1;
#if 1	// add by Victor Yu. 06-01-2005
	    t->Tm3Clock = 0;	// use PCLK
#endif
    	    break; 
        
        default:
        	break;   
	}

	return 1;
}

#if 0	// mask by Victor Yu. 01-31-2008
/* This routine stops the specified timer hardware. */
unsigned int cpe_timer_disable(unsigned int timer)
{
	cpe_time_ctrl_t *t=(cpe_time_ctrl_t *)(CPE_TIMER1_VA_BASE + TIMER_CR);
    
	if ((timer == 0) || (timer > MAX_TIMER))
		return 0;

	switch(timer)
	{
        case 1:
    	    t->Tm1En=0;
    	    t->Tm1OfEn=0;
    	    break;    
        case 2:
    	    t->Tm2En=0;
    	    t->Tm2OfEn=0;
    	    break; 
        case 3:
    	    t->Tm3En=0;
    	    t->Tm3OfEn=0;
    	    break; 
        
        default:
    	break;
   
	}

	return 1;
}
#endif

static void cpe_timer_set_counter(int timer, unsigned int value)
{
	volatile cpe_time_reg_t *t = TimerBase[timer];    
	t->TimerValue = value; 
}

static void cpe_timer_set_reload(int timer, unsigned int value)
{
	volatile cpe_time_reg_t *t = TimerBase[timer];    
	t->TimerLoad = value;   
#if 1	// add by Victor Yu. 01-31-2008
	t->TimerMatch1 = 0xffffffff;
	t->TimerMatch2 = 0xffffffff;
#endif
}

// --------------------------------------------------------------------
//	warning:
//		timer = 1, 2, 3
// --------------------------------------------------------------------
static unsigned long cpe_timer_get_counter(int timer)
{
	volatile cpe_time_reg_t *t = TimerBase[timer];
	return (volatile unsigned long)(t->TimerValue);
}

unsigned long cpe_gettimeoffset (void)
{
#if 1	// add by Victor Yu. 02-26-2007
	unsigned long	offsetticks;

	offsetticks = cpe_timer_get_counter(USED_TIMER);
	offsetticks = SET_COUNTER - offsetticks;
#if 0	// mask by Victor Yu. 01-31-2008
	if ( (*(volatile unsigned int *)(CPE_IC_VA_BASE+IRQ_STATUS_REG) & IRQ_TIMER1) ||
	     (*(volatile unsigned int *)(CPE_IC_VA_BASE+IRQ_STATUS_REG) & IRQ_TIMER1) ) {
#else
	if ( *(volatile unsigned int *)(CPE_TIMER1_VA_BASE+TIMER_INTR_STATE) ) {
		//printk("[has timer interrupt pending , %d !]\n", offsetticks);
#endif
		offsetticks = cpe_timer_get_counter(USED_TIMER);
		offsetticks = SET_COUNTER - offsetticks;
		offsetticks += SET_COUNTER;
	}
	offsetticks = offsetticks / (APB_CLK / 1000000);	// tansfer ticks to usec
	return offsetticks;
#else	// 02-26-2007
	return 0;
#endif	// 02-26-2007
}

#if 1	// add by Victor Yu. 05-25-2005
static irqreturn_t cpe_timer_interrupt(int irq, void *dev_id)
{
	//do_timer(regs);	// mask by Victor Yu. 06-01-2005
	timer_tick();	// add by Victor Yu. 06-01-2005
#if 1	// add by Victor Yu. 01-31-2008
	*(volatile unsigned int *)(CPE_TIMER1_VA_BASE+TIMER_INTR_STATE) = 0;
#endif
	return IRQ_HANDLED;
}

static struct irqaction cpe_timer_irq = {
	.name		= "Moxa CPE timer interrupt",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= cpe_timer_interrupt
};

void __init moxa_time_init(void)
{
//#ifdef TIMER_INC_MODE
#if 0	// mask by Victor Yu. 01-31-2008
	cpe_timer_set_reload(USED_TIMER, 0xffffffff - SET_COUNTER);
	cpe_timer_set_counter(USED_TIMER, 0xffffffff - SET_COUNTER);
#else
	cpe_timer_set_reload(USED_TIMER, SET_COUNTER);
	cpe_timer_set_counter(USED_TIMER, SET_COUNTER);
#endif

	if( !cpe_timer_enable(USED_TIMER) ) {
                panic("can not enable timer\n");
	}

	printk("IRQ timer at interrupt number 0x%x clock %d\r\n",IRQ_TIMER1,APB_CLK);
	//moxa_int_set_irq(IRQ_TIMER1, EDGE, L_ACTIVE);
	cpe_int_set_irq(IRQ_TIMER1, EDGE, L_ACTIVE);
	setup_irq(IRQ_TIMER1, &cpe_timer_irq);
}
#endif

struct sys_timer moxa_timer = {
	.init		= moxa_time_init,
	.offset		= cpe_gettimeoffset,
};
