struct counter_dev {
    uint8_t count;          /* Counter value */
    uint8_t size;           /* data size */
    struct semaphore sem;   /* mutual exclusion semaphore     */
	struct cdev cdev;	    /* Char device structure		*/
};

int counter_open(struct inode *inode, struct file *filp);
int counter_release(struct inode *inode, struct file *filp);
ssize_t counter_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t counter_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
void counter_cleanup_module(void);
int counter_init_module(void);