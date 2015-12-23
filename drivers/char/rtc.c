/*
 *	A simple generic Real Time Clock interface for Linux/Moxa R7000
 *
 * 	History:
 * 	Date		Author			Comment
 * 	11-27-2003	Victor Yu.		Create it.
 * 	01-26-2006	Jimmy_chen@moxa.com.tw	Copy UC7420 RTC and fix it to meet MOXART
 * 	02-09-2007	Victor Yu. 		Porting to kernel 2.6.19. Interrupt kernel API changed.
 * 	09-05-2007	Victor Yu.		Change to use raw level gpio API.
 */

#define RTC_VERSION		"1.0"

//#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/rtc.h>

#include <asm/io.h>
#include <mach/hardware.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <mach/moxa.h>
#include <mach/gpio.h>
#if defined(CONFIG_ARCH_EM1240) || defined(CONFIG_ARCH_EM1240_IVTC) || defined(CONFIG_ARCH_EM1240_MT)
#define GPIO_RTC_SCLK		(1<<20)  /* GPIO5 */
#define GPIO_RTC_DATA		(1<<21)  /* GPIO6 */
#define GPIO_RTC_RESET		(1<<9)  /* GPIO7 */
#elif defined(CONFIG_ARCH_W311_TEST)	// add by Victor Yu. 07-06-2008
#define GPIO_RTC_SCLK		(1<<24)  /* GPIO5 */
#define GPIO_RTC_DATA		(1<<25)  /* GPIO6 */
#define GPIO_RTC_RESET		(1<<26)  /* GPIO7 */
#else
#define GPIO_RTC_SCLK		(1<<5)  /* GPIO5 */
#define GPIO_RTC_DATA		(1<<6)  /* GPIO6 */
#define GPIO_RTC_RESET		(1<<7)  /* GPIO7 */
#endif
#define GPIO_RTC_MASK		(GPIO_RTC_SCLK|GPIO_RTC_DATA|GPIO_RTC_RESET)
#define MOXA_GPIO_PPENABLE	0x18
#define MOXA_GPIO_PPTYPE	0x1C
#define MOXA_GPIO_DATAOUT	0x0
#define MOXA_GPIO_DATAIN	0x04
#define MOXA_GPIO_PINDIR	0x08
#define GPIO_PINDIR_HIGH	1
#define GPIO_PINDIR_LOW		0
#define EM1240_GPIO_HIGH	1
#define EM1240_GPIO_LOW		0  
#define RTC_PROTECT_W   	0x8E
#define RTC_PROTECT_R   	0x8F
#define RTC_YEAR_W      	0x8C
#define RTC_YEAR_R      	0x8D
#define RTC_DAY_W       	0x8A
#define RTC_DAY_R       	0x8B
#define RTC_MONTH_W     	0x88
#define RTC_MONTH_R     	0x89
#define RTC_DATE_W      	0x86
#define RTC_DATE_R      	0x87
#define RTC_HOURS_W     	0x84
#define RTC_HOURS_R     	0x85
#define RTC_MINUTES_W 		0x82
#define RTC_MINUTES_R 		0x83
#define RTC_SECONDS_W 		0x80
#define RTC_SECONDS_R 		0x81
#define RTC_DELAY_TIME  	8       // 8 usecond
#define RTC_IS_OPEN             0x01	/* means /dev/rtc is in use     */
#define PIO(x)			1<<x

static void	gpio_line_inout(unsigned long gpio, int state)
{
	if(state){/* 1:output, 0:input */
		mcpu_gpio_inout(gpio, MCPU_GPIO_OUTPUT);
	}else{
		mcpu_gpio_inout(gpio, MCPU_GPIO_INPUT);
	}
}

static void 	gpio_line_set(unsigned long gpio, int status)
{
	if(status){
		mcpu_gpio_set(gpio, MCPU_GPIO_HIGH);
	} else{
		mcpu_gpio_set(gpio, MCPU_GPIO_LOW);
	}
}

static void	gpio_line_get(int gpio, int *value)
{
	if ( mcpu_gpio_get(gpio) )
		*value = 1;
	else
		*value = 0;
}
static unsigned long rtc_status = 0;	/* bitmapped status byte.       */

static int rtc_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data);
static spinlock_t rtc_lock;
static unsigned long epoch = 2000;	/* year corresponding to 0x00	*/

static const unsigned char days_in_mo[] = 
{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static u8 RTCReadRegister(u8 Cmd)
{
    u8		data;
    int 	i, v;
    unsigned long	flags;
    
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 9)	// chagnged by Victor Yu. 02-09-2007
    local_irq_save(flags);
#else
    save_flags(flags);
    cli();
#endif
    gpio_line_inout(GPIO_RTC_DATA, GPIO_PINDIR_HIGH);
    gpio_line_set(GPIO_RTC_RESET, EM1240_GPIO_HIGH);
    udelay(RTC_DELAY_TIME);
	/* write command byte */
    for ( i=0; i<8; i++, Cmd>>=1 ){
    	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_LOW);
    	if ( Cmd & 1 )
    		gpio_line_set(GPIO_RTC_DATA, EM1240_GPIO_HIGH);
    	else
    		gpio_line_set(GPIO_RTC_DATA, EM1240_GPIO_LOW);
    	udelay(RTC_DELAY_TIME);
    	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_HIGH);
    	udelay(RTC_DELAY_TIME);
    }
    
    gpio_line_inout(GPIO_RTC_DATA, GPIO_PINDIR_LOW);
    /* read data byte */
    udelay(RTC_DELAY_TIME);
    for ( i=0,data=0; i<8; i++ ){
    	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_LOW);
    	udelay(RTC_DELAY_TIME);
	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_HIGH);
	gpio_line_get(GPIO_RTC_DATA, &v);
    	if ( v )
    		data |= (1<<i);
    	udelay(RTC_DELAY_TIME);
    }
    gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_LOW);
    gpio_line_set(GPIO_RTC_RESET, EM1240_GPIO_LOW);
    udelay(RTC_DELAY_TIME);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 9)	// changed by Victor Yu, 02-09-2007
    local_irq_restore(flags);
#else
    restore_flags(flags);
#endif

    return data;
}

static void RTCWriteRegister(u8 Cmd, u8 Data)
{
    int 	i;
    unsigned long	flags;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 9)	// chagnged by Victor Yu. 02-09-2007
    local_irq_save(flags);
#else
    save_flags(flags);
    cli();
#endif
    gpio_line_inout(GPIO_RTC_DATA, GPIO_PINDIR_HIGH);
    gpio_line_set(GPIO_RTC_RESET, EM1240_GPIO_HIGH);
    udelay(RTC_DELAY_TIME);
    /* write command byte */
    for ( i=0; i<8; i++,Cmd>>=1 ) {
    	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_LOW);
    	if ( Cmd & 1 )
    		gpio_line_set(GPIO_RTC_DATA, EM1240_GPIO_HIGH);
    	else
    		gpio_line_set(GPIO_RTC_DATA, EM1240_GPIO_LOW);
    	udelay(RTC_DELAY_TIME);
        gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_HIGH);
    	udelay(RTC_DELAY_TIME);
    }
    
    /* write data byte */
    gpio_line_inout(GPIO_RTC_DATA, GPIO_PINDIR_HIGH);
    for ( i=0; i<8; i++,Data>>=1 ){
    	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_LOW);
    	if ( Data & 1 )
    		gpio_line_set(GPIO_RTC_DATA, EM1240_GPIO_HIGH);
    	else
    		gpio_line_set(GPIO_RTC_DATA, EM1240_GPIO_LOW);
    	udelay(RTC_DELAY_TIME);
    	gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_HIGH);
    	udelay(RTC_DELAY_TIME);
    }
    gpio_line_set(GPIO_RTC_SCLK, EM1240_GPIO_LOW);
    gpio_line_set(GPIO_RTC_RESET, EM1240_GPIO_LOW);
    udelay(RTC_DELAY_TIME);
    gpio_line_inout(GPIO_RTC_DATA, GPIO_PINDIR_LOW);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 9)	// changed by Victor Yu, 02-09-2007
    local_irq_restore(flags);
#else
    restore_flags(flags);
#endif
}

#if 1	// add by Victor Yu. 01-10-2005
static int	day_of_year[12]={0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
#endif
static void get_rtc_time(struct rtc_time *rtc_tm)
{
	unsigned char	v;
	
	spin_lock_irq(&rtc_lock);
	v = RTCReadRegister(RTC_SECONDS_R);
	rtc_tm->tm_sec = (((v & 0x70) >> 4) * 10) + (v & 0x0F);
	v = RTCReadRegister(RTC_MINUTES_R);
	rtc_tm->tm_min = (((v & 0x70) >> 4) * 10) + (v & 0x0F);
	v = RTCReadRegister(RTC_HOURS_R);
	if ( v & 0x80 ) {	// 12-hour mode
		rtc_tm->tm_hour = (((v & 0x10) >> 4) * 10) + (v & 0x0F);
		if ( v & 0x20 ) {	// PM mode
			rtc_tm->tm_hour += 12;
			if ( rtc_tm->tm_hour >= 24 )
				rtc_tm->tm_hour = 0;
		}
	} else {	// 24-hour mode
		rtc_tm->tm_hour = (((v & 0x30) >> 4) * 10) + (v & 0x0F);
	}
	v = RTCReadRegister(RTC_DATE_R);
	rtc_tm->tm_mday = (((v & 0x30) >> 4) * 10) + (v & 0x0F);
	v = RTCReadRegister(RTC_MONTH_R);
	rtc_tm->tm_mon = (((v & 0x10) >> 4) * 10) + (v & 0x0F);
	rtc_tm->tm_mon--;
	v = RTCReadRegister(RTC_YEAR_R);
	rtc_tm->tm_year = (((v & 0xF0) >> 4) * 10) + (v & 0x0F);
	if ((rtc_tm->tm_year += (epoch - 1900)) <= 69)
		rtc_tm->tm_year += 100;
#if 1   // add by Victor Yu. 01-10-2005
	v = RTCReadRegister(RTC_DAY_R);
	rtc_tm->tm_wday = (v & 0x0f) - 1;
	rtc_tm->tm_yday = day_of_year[rtc_tm->tm_mon];
	rtc_tm->tm_yday += (rtc_tm->tm_mday-1);
	if ( rtc_tm->tm_mon >= 2 ) {
		if ( !(rtc_tm->tm_year % 4) && (rtc_tm->tm_year % 100) )
			rtc_tm->tm_yday++;
	}
	rtc_tm->tm_isdst = 0;
#endif
	spin_unlock_irq(&rtc_lock);
}

static long
rtc_ioctl(struct file *file, unsigned int cmd,
	  unsigned long arg)
{
	struct rtc_time rtc_tm;
	unsigned char	v;

	switch (cmd) {
	case RTC_RD_TIME:	/* Read the time/date from RTC  */
		get_rtc_time(&rtc_tm);
		return copy_to_user((void *) arg, &rtc_tm, sizeof(rtc_tm)) ? 
			-EFAULT : 0;
	case RTC_SET_TIME:	/* Set the RTC */
	{
		unsigned char mon, day, hrs, min, sec, leap_yr;
		unsigned int yrs;
		
		if (!capable(CAP_SYS_TIME))
			return -EACCES;

		if (copy_from_user(&rtc_tm, 
				   (struct rtc_time *) arg,
		                   sizeof(struct rtc_time))) 
			return -EFAULT;
			
		yrs = rtc_tm.tm_year + 1900;
		mon = rtc_tm.tm_mon + 1;   /* tm_mon starts at zero */
		day = rtc_tm.tm_mday;
		hrs = rtc_tm.tm_hour;
		min = rtc_tm.tm_min;
		sec = rtc_tm.tm_sec;

		if (yrs < 1970)
			return -EINVAL;

		leap_yr = ((!(yrs % 4) && (yrs % 100)) || !(yrs % 400));

		if ((mon > 12) || (day == 0))
			return -EINVAL;

		if (day > (days_in_mo[mon] + ((mon == 2) && leap_yr)))
			return -EINVAL;
			
		if ((hrs >= 24) || (min >= 60) || (sec >= 60))
			return -EINVAL;

		if ((yrs -= epoch) > 255)    /* They are unsigned */
			return -EINVAL;
		spin_lock_irq(&rtc_lock);
		/* These limits and adjustments are independant of
		 * whether the chip is in binary mode or not.
		 */
		if (yrs > 169) {
			spin_unlock_irq(&rtc_lock);
			return -EINVAL;
		}
		if (yrs >= 100)
			yrs -= 100;
			
		RTCWriteRegister(RTC_PROTECT_W, 0);
		v = ((hrs / 10) << 4) | (hrs % 10);
		RTCWriteRegister(RTC_HOURS_W, v);
		v = ((min / 10) << 4) | (min % 10);
		RTCWriteRegister(RTC_MINUTES_W, v);
		v = ((sec / 10) << 4) | (sec % 10);
		RTCWriteRegister(RTC_SECONDS_W, v);
		v = ((yrs / 10) << 4) | (yrs % 10);
		RTCWriteRegister(RTC_YEAR_W, v);
		v = ((mon / 10) << 4) | (mon % 10);
		RTCWriteRegister(RTC_MONTH_W, v);
		v = ((day / 10) << 4) | (day % 10);
		RTCWriteRegister(RTC_DATE_W, v);
		RTCWriteRegister(RTC_PROTECT_W, 0x80);
		
		spin_unlock_irq(&rtc_lock);
		return 0;
	}
	default:
		return -EINVAL;
	}
}

/* We use rtc_lock to protect against concurrent opens. So the BKL is not
 * needed here. Or anywhere else in this driver. */
static int rtc_open(struct inode *inode, struct file *file)
{
	spin_lock_irq(&rtc_lock);

	if (rtc_status & RTC_IS_OPEN) {
		spin_unlock_irq(&rtc_lock);
		return -EBUSY;
	}

	rtc_status |= RTC_IS_OPEN;

	spin_unlock_irq(&rtc_lock);
	return 0;
}

static int rtc_release(struct inode *inode, struct file *file)
{
	spin_lock_irq(&rtc_lock);
	rtc_status &= ~RTC_IS_OPEN;
	spin_unlock_irq(&rtc_lock);
	return 0;
}

/*
 *	The various file operations we support.
 */

static struct file_operations rtc_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.unlocked_ioctl = rtc_ioctl,
	.open = rtc_open,
	.release = rtc_release,
};

static struct miscdevice rtc_dev = {
	RTC_MINOR,
	"rtc",
	&rtc_fops
};

#define PMU_GPIO_ON	0xdfc003f0
static int __init rtc_init(void)
{
	struct rtc_time rtc_tm;

	mcpu_gpio_mp_set(GPIO_RTC_MASK);
	gpio_line_inout(GPIO_RTC_RESET, GPIO_PINDIR_HIGH);
	gpio_line_inout(GPIO_RTC_SCLK, GPIO_PINDIR_HIGH);

#if 1	// add by Victor Yu. 04-21-2005, to avoid the RTS stop
	get_rtc_time(&rtc_tm);
//printk("YYYY-MON-DAY-HH-MM-SS=%d-%d-%d-%d-%d-%d\n", rtc_tm.tm_year, rtc_tm.tm_mon, rtc_tm.tm_mday, rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	if ( rtc_tm.tm_sec == 0 && rtc_tm.tm_min == 0 && rtc_tm.tm_min == 0 && rtc_tm.tm_hour == 0 && rtc_tm.tm_year == 100 && rtc_tm.tm_mon == 0 && rtc_tm.tm_mday == 1 ) {
		printk("The RTC has stoped. Now reenable it.\n");
		RTCWriteRegister(RTC_PROTECT_W,0);/* Disable Write Protect */
		RTCWriteRegister(RTC_SECONDS_W,0);/* Enable OSC */
		RTCWriteRegister(RTC_PROTECT_W,0x80);/* Enable Write Protect */
	}
#endif
	misc_register(&rtc_dev);
	create_proc_read_entry("driver/rtc", 0, 0, rtc_read_proc, NULL);

	printk(KERN_INFO "Generic Moxa RTC Driver v" RTC_VERSION "\n");
	return 0;
}

static void __exit rtc_exit(void)
{
	remove_proc_entry("driver/rtc", NULL);
	misc_deregister(&rtc_dev);

}

module_init(rtc_init);
module_exit(rtc_exit);

/*
 *	Info exported via "/proc/driver/rtc".
 */

static int rtc_proc_output(char *buf)
{
	char *p;
	struct rtc_time tm;

	get_rtc_time(&tm);

	p = buf;

	/*
	 * There is no way to tell if the luser has the RTC set for local
	 * time or for Universal Standard Time (GMT). Probably local though.
	 */
	p += sprintf(p,
		     "rtc_time\t: %02d:%02d:%02d\n"
		     "rtc_date\t: %04d-%02d-%02d\n"
	 	     "rtc_epoch\t: %04lu\n",
		     tm.tm_hour, tm.tm_min, tm.tm_sec,
		     tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, epoch);

	return p - buf;
}

static int rtc_read_proc(char *page, char **start, off_t off,
			 int count, int *eof, void *data)
{
	int len = rtc_proc_output(page);
	if (len <= off + count)
		*eof = 1;
	*start = page + off;
	len -= off;
	if (len > count)
		len = count;
	if (len < 0)
		len = 0;
	return len;
}

MODULE_AUTHOR("Victor Yu");
MODULE_LICENSE("GPL");
