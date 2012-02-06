/*
 * Lilith uio driver.
 *
 * Export at91 GPIO, Timer, ... registers to userspace.
 *
 * Copyright (C) 2010-11 Texas Instruments Incorporated - http://www.ti.com/
 * Copyright (C) 2012 Julien Rouviere
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/io.h>
#include <linux/slab.h>

#define DRV_NAME 	"lilith_uio"
#define DRV_VERSION 	"1.0"


struct uio_lilith_dev {
	struct uio_info info;
};

static irqreturn_t lilith_handler(int irq, struct uio_info *info)
{
	struct uio_lilith_dev *gdev = info->priv;

	return IRQ_HANDLED;
}

static void lilith_cleanup(struct platform_device *dev,
			struct uio_lilith_dev *gdev)
{

}

static int __devinit lilith_probe(struct platform_device *dev)
{
	struct uio_info *p;
	struct uio_lilith_dev *gdev;
	struct resource *regs_prussio;

	int ret = -ENODEV, cnt = 0, len;

	gdev = kzalloc(sizeof(struct uio_lilith_dev), GFP_KERNEL);
	if (!gdev) {
		return -ENOMEM;
	}


	regs_prussio = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!regs_prussio) {
		dev_err(&dev->dev, "No PRUSS I/O resource specified\n");
		goto out_free;
	}

	if (!regs_prussio->start) {
		dev_err(&dev->dev, "Invalid memory resource\n");
		goto out_free;
	}

	len = resource_size(regs_prussio);
	gdev->prussio_vaddr = ioremap(regs_prussio->start, len);
	if (!gdev->prussio_vaddr) {
		dev_err(&dev->dev, "Can't remap PRUSS I/O  address range\n");
		goto out_free;
	}


	platform_set_drvdata(dev, gdev);
	return 0;

out_free:
	lilith_cleanup(dev, gdev);
	return ret;
}

static int __devexit lilith_remove(struct platform_device *dev)
{
	struct uio_lilith_dev *gdev = platform_get_drvdata(dev);

	lilith_cleanup(dev, gdev);
	platform_set_drvdata(dev, NULL);
	return 0;
}

static struct platform_driver lilith_driver = {
	.probe = lilith_probe,
	.remove = __devexit_p(lilith_remove),
	.driver = {
		   .name = DRV_NAME,
		   .owner = THIS_MODULE,
		   },
};

module_platform_driver(lilith_driver);

MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Julien Rouviere");

