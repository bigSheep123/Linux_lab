#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/string.h>
#include <linux/device.h>

// 文件操作，设备的文件操作
// open write read close 

static struct cdev *my_cdev;
static struct class * dh_dev;
static dev_t dev = 0;
char* shared_buf = NULL;

#define DEVNAME "dhDev"
MODULE_LICENSE("Dual BSD/GPL");

static int fp_open(struct inode *node,struct file *f)
{
    printk(KERN_INFO "Start to Open File!");
    return 0;
}

static ssize_t fp_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Start to read the kernel\n");
    if (shared_buf == NULL)
    {
        printk(KERN_ERR "Should Write Before Read!!!\n");
        return 0;
    }
    if (copy_to_user(buf, shared_buf, strlen(shared_buf)))
    {
        printk(KERN_ERR "Copy Failed\n");
        return 0;
    }
    return strlen(shared_buf);
}

static ssize_t fp_write(struct file* f,const char* buf, size_t len, loff_t* off)
{
    printk(KERN_INFO "Start to write the Kernel\n");
    if( copy_from_user(shared_buf,buf,len+1))
    {
        printk(KERN_ERR "Copy Failed\n");
        return 0;
    }
    return strlen(shared_buf);
}

static int fp_release(struct inode *node, struct file *f) {   return 0;  }

static struct file_operations fp = {
    .owner = THIS_MODULE,
    .open = fp_open,
    .read = fp_read,
    .write = fp_write,
    .release = fp_release,
};

void Tranverse(void )
{
    printk(KERN_INFO "名称\t\t进程号\t\t状态\t\t优先级\t\t父进程号\n");
    struct task_struct * p = NULL;
    for_each_process(p) {
        if(p)
            printk(KERN_INFO "%-20s%-20d%-20d%-20d%-20d\n",p->comm,p->pid,p->stats,p->normal_prio,p->parent->pid); 
    }
    return ;
}

static int __init my_init(void) {
    printk(KERN_ERR "Start Init\n");

    my_cdev = cdev_alloc();  // cdev_alloc() 分配cdev结构
    cdev_init(my_cdev,&fp);  // 初始化 cdev 结构
    if (cdev_add(my_cdev,dev,1)) {
        printk(KERN_ERR "Add Error\n");
        return -1;
    }

    if( alloc_chrdev_region(&dev,0,1,DEVNAME)) {
        printk(KERN_ERR "Alloc Error\n");
        return -1;
    }
	
    dh_dev = class_create(THIS_MODULE,DEVNAME);
    if (dh_dev == NULL) {
        printk(KERN_ERR "Add Error\n");
        return -1;
    }

    if (!device_create(dh_dev, NULL, dev, NULL, DEVNAME))
    {
        printk(KERN_ERR "Device Create Error\n");
        return -1;
    }

    printk(KERN_INFO "Driver Insert In!!!\n");
    shared_buf = kmalloc(1024, GFP_KERNEL);
    memset(shared_buf, 0, 1024);
    Tranverse();
    return 0;
}

static void __exit my_exit(void)
{
    device_destroy(dh_dev, dev);
    class_destroy(dh_dev);
    cdev_del(my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_ERR "Success Exit!\n");
}

module_init(my_init);
module_exit(my_exit);