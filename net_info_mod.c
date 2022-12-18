#include <linux/delay.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/sched.h>

#include "mod.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_FILE_NAME "net_info"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("locci7");
MODULE_DESCRIPTION("Lab 2 module sample.");

static struct proc_dir_entry *proc_file;
static struct net_device *dev;

static ssize_t proc_file_write(struct file *, const char *, size_t, loff_t *);
static ssize_t proc_file_read(struct file *, char *, size_t, loff_t *);

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_ops = {
	.proc_write = proc_file_write,
	.proc_read = proc_file_read,
};
#else
static const struct file_operations proc_file_ops = {
	.write = proc_file_write,
	.read = proc_file_read,q
};
#endif

static int __init proc_info_mod_init(void) {
	proc_file = proc_create(PROC_FILE_NAME, 0666, NULL, &proc_file_ops);
	if (NULL == proc_file) {
		proc_remove(proc_file);
		pr_alert("ERROR: could not initialize /proc/%s\n", PROC_FILE_NAME);
		return -ENOMEM;
	}
	pr_info("/proc/%s\n", PROC_FILE_NAME);
	
	return 0;
}

static void __exit proc_info_mod_exit(void) {
	remove_proc_entry(PROC_FILE_NAME, NULL);
	pr_debug("/proc/%s removed.", PROC_FILE_NAME);
}

module_init(proc_info_mod_init);
module_exit(proc_info_mod_exit);

static ssize_t proc_file_read(struct file *file, char *user, size_t lenght, loff_t *offset) {
	struct net_device_info info;
	size_t r_lenght;
	
	pr_info("proc file is reading.");
	
	if (*offset > 0) return 0;
	
	if (dev == NULL) {
		pr_alert("can't find dev from name.");
		return 0;
	}
	else {
		info.mem_start = dev->mem_start;
		info.mem_end = dev->mem_end;
		info.base_addr = dev->base_addr;
		info.state = dev->state;
		
		r_lenght = sizeof(struct net_device_info);
	}
	
	if (copy_to_user(user, &info, r_lenght))
		return -EFAULT;
	
	pr_info("info wrote.");
	
	*offset = r_lenght; 
	return r_lenght;
}

static ssize_t proc_file_write(struct file *file, const char *user, size_t lenght, loff_t *offset) {

	char *dev_name = kmalloc(lenght, GFP_KERNEL);

	if (*offset > 0) {
		return 0;
	}
	
	if (copy_from_user(dev_name, user, lenght))
		return -EFAULT;
		
	pr_info("net device name: %s", dev_name);
	
	read_lock(&dev_base_lock);
	
	dev = dev_get_by_name(&init_net, dev_name);
	
	read_unlock(&dev_base_lock);
	
	*offset = strlen(dev_name);
	return strlen(dev_name);
}
