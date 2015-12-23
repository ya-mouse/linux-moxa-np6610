
//#include	<linux/config.h>
#include	<linux/module.h>
#include	<linux/kernel.h>
#include	<linux/types.h>
#include	<linux/spinlock.h>

#include	<asm/io.h>
#include	<mach/moxa.h>
#include	<mach/gpio.h>

#define VICTOR_USE_LOCK_IRQ
static mcpu_gpio_reg_t	*gpio_reg=(mcpu_gpio_reg_t *)CPE_GPIO_VA_BASE;
static DEFINE_SPINLOCK(gpio_lock);

/*
 * To set the GPIO mode for INPUT or OUTPUT
 */
void	mcpu_gpio_inout(u32 gpio, int inout)
{
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	unsigned long	flags;
#endif

#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_lock_irqsave(&gpio_lock, flags);
#else
	spin_lock(&gpio_lock);
#endif
	switch ( inout ) {
	case MCPU_GPIO_INPUT :
		writel(readl(&gpio_reg->pin_dir)&~gpio, &gpio_reg->pin_dir);
		break;
	case MCPU_GPIO_OUTPUT :
		writel(readl(&gpio_reg->pin_dir)|gpio, &gpio_reg->pin_dir);
		break;
	}
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_unlock_irqrestore(&gpio_lock, flags);
#else
	spin_unlock(&gpio_lock);
#endif
}
EXPORT_SYMBOL(mcpu_gpio_inout);

/*
 * To get the GPIO which mode, INPUT or OUTPUT
 */
u32	mcpu_gpio_get_inout(u32 gpio)
{
	u32	ret;
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	unsigned long	flags;
#endif

#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_lock_irqsave(&gpio_lock, flags);
#else
	spin_lock(&gpio_lock);
#endif
	if ( readl(&gpio_reg->pin_dir) & gpio )
		ret = MCPU_GPIO_OUTPUT;
	else
		ret = MCPU_GPIO_INPUT;
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_unlock_irqrestore(&gpio_lock, flags);
#else
	spin_unlock(&gpio_lock);
#endif
	return ret;
}
EXPORT_SYMBOL(mcpu_gpio_get_inout);

/*
 * To set the GPIO ouput signal to high or low
 */
void	mcpu_gpio_set(u32 gpio, int highlow)
{
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	unsigned long	flags;
#endif

#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_lock_irqsave(&gpio_lock, flags);
#else
	spin_lock(&gpio_lock);
#endif
	switch ( highlow ) {
	case MCPU_GPIO_HIGH :
		writel(readl(&gpio_reg->data_out)|gpio, &gpio_reg->data_out);
		break;
	case MCPU_GPIO_LOW :
		writel(readl(&gpio_reg->data_out)&~gpio, &gpio_reg->data_out);
		break;
	}
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_unlock_irqrestore(&gpio_lock, flags);
#else
	spin_unlock(&gpio_lock);
#endif
}
EXPORT_SYMBOL(mcpu_gpio_set);

/*
 * to get the GPIO input which level, high or low
 */
u32	mcpu_gpio_get(u32 gpio)
{
	u32	ret;
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	unsigned long	flags;
#endif

#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_lock_irqsave(&gpio_lock, flags);
#else
	spin_lock(&gpio_lock);
#endif
#if 0	// mask by Victor Yu. 04-20-2007
	ret = readl(&gpio_reg->data_in) & gpio;
#else
	ret = readl(&gpio_reg->pin_dir);
	if ( ret & gpio )	// this is output GPIO
		ret = readl(&gpio_reg->data_out) & gpio;
	else
		ret = readl(&gpio_reg->data_in) & gpio;
#endif
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_unlock_irqrestore(&gpio_lock, flags);
#else
	spin_unlock(&gpio_lock);
#endif
	return ret;
}
EXPORT_SYMBOL(mcpu_gpio_get);

/* 
 * To set thie GPIO to work GPIO mode 
 */
void	mcpu_gpio_mp_set(u32 gpio)
{
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	unsigned long	flags;
#endif

#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_lock_irqsave(&gpio_lock, flags);
#else
	spin_lock(&gpio_lock);
#endif
	*(volatile unsigned int *)(CPE_PMU_VA_BASE+0x100) |= gpio;
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_unlock_irqrestore(&gpio_lock, flags);
#else
	spin_unlock(&gpio_lock);
#endif
}
EXPORT_SYMBOL(mcpu_gpio_mp_set);

/*
 * To set the GPIO for component mode
 */
void	mcpu_gpio_mp_clear(u32 gpio)
{
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	unsigned long	flags;
#endif

#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_lock_irqsave(&gpio_lock, flags);
#else
	spin_lock(&gpio_lock);
#endif
	*(volatile unsigned int *)(CPE_PMU_VA_BASE+0x100) &= ~gpio;
#ifdef VICTOR_USE_LOCK_IRQ	// add by Victor Yu. 07-25-2007
	spin_unlock_irqrestore(&gpio_lock, flags);
#else
	spin_unlock(&gpio_lock);
#endif
}
EXPORT_SYMBOL(mcpu_gpio_mp_clear);
