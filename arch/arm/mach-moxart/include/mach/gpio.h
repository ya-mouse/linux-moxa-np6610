
#ifndef _MOXACPU_GPIO_H
#define _MOXACPU_GPIO_H

typedef struct _mcpu_gpio_reg_struct {
	unsigned int	data_out;
	unsigned int	data_in;
	unsigned int	pin_dir;	// 1 for output, 0 for input
	unsigned int	reserved;
	unsigned int	data_set;
	unsigned int	data_clear;
	unsigned int	pin_pull_enable;
	unsigned int	pin_pull_type;
	unsigned int	int_enable;
	unsigned int	int_raw_state;
	unsigned int	int_masked_state;
	unsigned int	int_mask;
	unsigned int	int_clear;
	unsigned int	int_trigger;
	unsigned int	int_both;
	unsigned int	int_rise_neg;
	unsigned int	bounce_enable;
	unsigned int	bounce_pre_scale;
} mcpu_gpio_reg_t;

#define MCPU_GPIO_INPUT		0
#define MCPU_GPIO_OUTPUT	1
#define MCPU_GPIO_HIGH		1
#define MCPU_GPIO_LOW		0

extern void	mcpu_gpio_inout(u32 gpio, int inout);
extern u32	mcpu_gpio_get_inout(u32 gpio);
extern void	mcpu_gpio_set(u32 gpio, int highlow);
extern u32	mcpu_gpio_get(u32 gpio);
extern void	mcpu_gpio_mp_set(u32 gpio);
extern void	mcpu_gpio_mp_clear(u32 gpio);

#endif	// _MOXACPU_GPIO_H
