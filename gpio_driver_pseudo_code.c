/*
 * Copyright (c) 2009-2014 Lexistech.
 *		http://www.lexistech.in/
 *
 * LEXISTECH - GPIO support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

struct lx_gpio_platform_data 
{
	int base;
	int ngpio;
};

static struct lx_gpio_platform_data lx_gpio_device_data = {
	.base	= 128,
	.ngpio	= 32,
};

static struct platform_device lx_gpio_device = {
	.name			= "lx_gpio",
	.id				= 0,
	.dev			= {
		.platform_data	= &lx_gpio_device_data,
	},
};

static int __init lx_gpio_init(void)
{
	return platform_device_register(&lx_gpio_device);
}

static void __exit lx_gpio_exit(void)
{
	platform_device_unregister(&lx_gpio_device);
}

module_init(lx_gpio_init);
module_exit(lx_gpio_exit);

MODULE_AUTHOR("Shilesh");
MODULE_DESCRIPTION("Shilesh GPIO device");
MODULE_LICENSE("GPL");
