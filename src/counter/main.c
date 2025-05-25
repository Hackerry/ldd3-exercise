#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

// #include <asm/system.h>		/* cli(), *_flags */
// #include <asm/uaccess.h>	/* copy_*_user */

#include "counter.h"		/* local definitions */

MODULE_AUTHOR("Hackerry");
MODULE_LICENSE("Dual BSD/GPL");

const int NUM_COUNTER_DEVICES = 4;

int counter_major = 0;
int counter_minor = 0;

struct counter_dev* counter_devices;

int counter_open(struct inode *inode, struct file *filp) {
	struct counter_dev *dev; /* device information */

	dev = container_of(inode->i_cdev, struct counter_dev, cdev);
	filp->private_data = dev; /* for other methods */
	return 0;          /* success */
}

int counter_release(struct inode *inode, struct file *filp) {
	return 0;
}

ssize_t counter_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    struct counter_dev *dev = filp->private_data;
    ssize_t retval = 0;

    // Acquire mutex
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    // Check readpos = 0 and count = 1
    if(*f_pos != 0) {
        // printk(KERN_WARNING "Fpos is not 0\n");
        goto out;
    }
    if(count > dev->size) {
        count = dev->size; // Limit count to size
        // printk(KERN_WARNING "Count %ld is greater than size\n", count);
    }

    if(copy_to_user(buf, &dev->count, count)) {
        retval = -EFAULT;
        goto out;
    }

    // Increment counter, wrap at 256
    dev->count = (dev->count + 1) % 256;

    *f_pos += count; // Update file position to let caller know
    retval = count;

out:
    up(&dev->sem);
    return retval;
}

ssize_t counter_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    struct counter_dev *dev = filp->private_data;
    ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if(*f_pos != 0)
        goto out;
    if(count > dev->size)
        count = dev->size; // Limit count to size

    if(copy_from_user(&dev->count, buf, count)) {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count; // Update file position to let caller know
    retval = count;

out:
    up(&dev->sem);
    return retval;
}

struct file_operations counter_fops = {
	.owner =    THIS_MODULE,
	// .llseek =   scull_llseek,
	.read =     counter_read,
	.write =    counter_write,
	// .unlocked_ioctl = scull_ioctl,
	.open =     counter_open,
	.release =  counter_release,
};

static void counter_setup_cdev(struct counter_dev *dev, int index) {
    int err, devno = MKDEV(counter_major, counter_minor + index);

    cdev_init(&dev->cdev, &counter_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &counter_fops;
    dev->size = 1;
    // 1 means only 1 device number is associated with this cdev
    // Once this line is executed, the device is live and kernel can immediately use it
    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
        printk(KERN_NOTICE "Error %d adding counter%d", err, index);
}

void counter_cleanup_module(void) {
	int i;
	dev_t devno = MKDEV(counter_major, counter_minor);

	/* Get rid of our char dev entries */
	if (counter_devices) {
		for (i = 0; i < NUM_COUNTER_DEVICES; i++) {
			cdev_del(&counter_devices[i].cdev);
		}
		kfree(counter_devices);
	}

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, NUM_COUNTER_DEVICES);
}

int counter_init_module(void) {
	int result, i;
	dev_t dev = 0;

    // Dynamic allocation of major number
    result = alloc_chrdev_region(&dev, counter_minor, NUM_COUNTER_DEVICES, "counter");
	if (result < 0) {
		printk(KERN_WARNING "counter: can't get major %d\n", counter_major);
		return result;
	}
    counter_major = MAJOR(dev);     // Set for clean up and reference in MKDEV

    // Allocate devices
	counter_devices = kmalloc(NUM_COUNTER_DEVICES * sizeof(struct counter_dev), GFP_KERNEL);
	if (!counter_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(counter_devices, 0, NUM_COUNTER_DEVICES * sizeof(struct counter_dev));

    /* Initialize each device. */
	for (i = 0; i < NUM_COUNTER_DEVICES; i++) {
		sema_init(&(counter_devices[i].sem), 1);
		counter_setup_cdev(&counter_devices[i], i);
	}

	return 0; /* succeed */

  fail:
    counter_cleanup_module();
	return result;
}

module_init(counter_init_module);
module_exit(counter_cleanup_module);