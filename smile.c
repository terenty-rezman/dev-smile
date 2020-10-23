/*                                                     
 * $Id: hello.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 */
#include <linux/init.h>
#include <linux/module.h>

#include <linux/cdev.h>   /* struct cdev */
#include <linux/fs.h>     /* alloc_chrdev_region(), struct file_operations */
#include <linux/kdev_t.h> /* MINOR() MAJOR() */
#include <linux/types.h>  /* dev_t */

#include <linux/slab.h>    /* kmalloc kfree */
#include <linux/uaccess.h> /* copy_from_user */

MODULE_LICENSE("Dual BSD/GPL");

const int count_devices = 1;

int smile_major = 0;
int smile_minor = 0;

/* prototypes */
static void smile_cleanup(void);

/* my struct representing the device */
/* coud be statically allocated */
struct smile_dev
{
    int initialized_ok; /* if device was succefully created */
    struct cdev cdev;   /* kernel char device structure */
}* smile_device = NULL; /* allocated in smile_init */

/* supported device operations */
ssize_t smile_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos);

ssize_t smile_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);

int smile_open(struct inode* inode, struct file* filp);

int smile_release(struct inode* inode, struct file* filp);

/* fill the struct with supported operations */
struct file_operations smile_fops = {
    .owner = THIS_MODULE,
    .read = smile_read,
    .write = smile_write,
    .open = smile_open,
    .release = smile_release,
};

const char smile_symbol[] = "\xF0\x9F\x98\x9D";
char* symbol_buffer = NULL;

int init_symbol_buffer(void)
{
    int i = 0;
    const int smile_symbol_size = sizeof(smile_symbol) / sizeof(smile_symbol[0]);
    const int symbol_buff_size = 2 * smile_symbol_size;

    symbol_buffer = kmalloc(symbol_buff_size, GFP_KERNEL);

    if (!symbol_buffer)
    {
        printk(KERN_ALERT "smile: cannot allocate memory for struct symbol_buffer\n");
        return -ENOMEM;
    }

    /* fill the buffer with smile symbols */
    for (i = 0; i < symbol_buff_size; ++i)
    {
        symbol_buffer[i] = smile_symbol[i % smile_symbol_size];
    }

	return 0;
}

static int smile_init(void)
{
    dev_t dev_numbers = 0;
    int err = 0;

    const int base_minor = 0;

    /* allocate device number */
    err = alloc_chrdev_region(&dev_numbers, base_minor, count_devices, "smile");

    if (err < 0)
    {
        printk(KERN_WARNING "smile: can't get major");
        /* return a negative error number to cause module loading to fail */
        /* smile_cleanup is not called */
        return err; /* fail */
    }

    smile_device = kmalloc(sizeof(struct smile_dev), GFP_KERNEL);
    if (!smile_device)
    {
        printk(KERN_ALERT "smile: cannot allocate memory for struct smile_device\n");
        err = -ENOMEM;
        goto fail;
    }
    memset(smile_device, 0, sizeof(struct smile_dev));

    /* initizlize the device */
    cdev_init(&smile_device->cdev, &smile_fops);
    smile_device->cdev.owner = THIS_MODULE;

    err = cdev_add(&smile_device->cdev, dev_numbers, 1);

    if (err)
    {
        printk(KERN_NOTICE "Error %d adding smile device", err);
        goto fail;
    }

    smile_device->initialized_ok = 1;

    smile_major = MAJOR(dev_numbers);
    printk(KERN_NOTICE "smile: device %d initialized\n", smile_major);

	err = init_symbol_buffer();
    if (err)
        goto fail;

    return 0; /* success */

fail:
    smile_cleanup();
    return err; /* fail */
}

static void smile_cleanup(void)
{
    dev_t dev = MKDEV(smile_major, smile_minor);

    if (smile_device)
        if (smile_device->initialized_ok)
            cdev_del(&smile_device->cdev); /* unregister the device */

    /* free dynamic memory if any allocated */
    if (smile_device)
        kfree(smile_device);

    if (symbol_buffer)
        kfree(symbol_buffer);

    /* smile_cleanup is never called if registering failed */
    unregister_chrdev_region(dev, count_devices);

    printk(KERN_NOTICE "smile: device %d unloaded\n", smile_major);
}

ssize_t smile_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
    const int smile_symbol_size = sizeof(smile_symbol) / sizeof(smile_symbol[0]);

    int read_pos = *f_pos % smile_symbol_size;
    int read_count = count > smile_symbol_size ? smile_symbol_size : count;

    copy_to_user(buf, symbol_buffer + read_pos, read_count);

    *f_pos += read_count;

    return read_count;
}

ssize_t smile_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)
{
    return count;
}

int smile_open(struct inode* inode, struct file* filp)
{
    return 0;
}

int smile_release(struct inode* inode, struct file* filp)
{
    return 0;
}

module_init(smile_init);
module_exit(smile_cleanup);
