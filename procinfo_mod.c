#include <linux/delay.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/netdevice.h>
#include <linux/path.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/sched.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_FILE_MAX_SIZE 256
#define BUF_INFO_MAX_SIZE 1000
#define PROC_FILE_NAME "proc_inf"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("locci7");
MODULE_DESCRIPTION("Lab 2 module sample.");

static struct proc_dir_entry *proc_file;
static struct inode *inode;

static ssize_t proc_file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t proc_file_write(struct file *, const char __user *, size_t, loff_t *);
static int proc_file_open(struct inode *, struct file *);
static int proc_file_close(struct inode *, struct file *);

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_ops = {
	.proc_read = proc_file_read,
	.proc_write = proc_file_write,
	.proc_open = proc_file_open,
	.proc_release = proc_file_close,
};
#else
static const struct file_operations proc_file_ops = {
	.read = proc_file_read,
	.write = proc_file_write,
	.open = proc_file_open,
	.release = proc_file_close,
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

static size_t get_net_device_info(char *user_info, size_t buff_seek) {
	struct net_device *dev;
	int dev_count = 1;
	
	buff_seek += sprintf(user_info + buff_seek, "----NET DEVICES INFO----\n");
	
	read_lock(&dev_base_lock);
	dev = first_net_device(&init_net);
	while(dev) {
		buff_seek += sprintf(user_info + buff_seek, "--DEVICE %d--\n", dev_count);
		buff_seek += sprintf(user_info + buff_seek, "device name: %s\n", dev->name);
		buff_seek += sprintf(user_info + buff_seek, "device memory start: %ld\n", dev->mem_start);
		buff_seek += sprintf(user_info + buff_seek, "device memory end: %ld\n", dev->mem_end);
		buff_seek += sprintf(user_info + buff_seek, "device I/O address: %lu\n", dev->base_addr);
		buff_seek += sprintf(user_info + buff_seek, "device state: %lu\n", dev->state);
		buff_seek += sprintf(user_info + buff_seek, "-------------\n");
		dev = next_net_device(dev);
		dev_count++;
	}
	
	read_unlock(&dev_base_lock);

	pr_debug("get_net_device_info: net_device info wrote.");
	return buff_seek;
}

static ssize_t proc_file_read(struct file *file, char __user *user, size_t lenght, loff_t *offset) {
	char kern_info[BUF_INFO_MAX_SIZE];
	size_t buff_seek = 0;
	
	pr_info("proc file is reading.");
	
	if (*offset > 0) return 0;
	
	if (inode == NULL) {
		pr_alert("can't find inode from path.");
	}
	else {
		buff_seek += sprintf(kern_info + buff_seek, "----INODE INFO----\n");
		buff_seek += sprintf(kern_info + buff_seek, "inode number: %lu\n", inode->i_ino);
		buff_seek += sprintf(kern_info + buff_seek, "inode references count: %d\n", inode->i_count.counter);
		buff_seek += sprintf(kern_info + buff_seek, "inode access permissions: %llu\n", (unsigned long long)inode->i_mode);
		buff_seek += sprintf(kern_info + buff_seek, "inode file size: %lld\n", inode->i_size);
		buff_seek += sprintf(kern_info + buff_seek, "inode file last modify time: %.9ld UTS\n", inode->i_mtime.tv_nsec);
		buff_seek += sprintf(kern_info + buff_seek, "-------------\n");	
		pr_info("inode wrote.");	
	}
	
	buff_seek += get_net_device_info(kern_info, buff_seek);
	
	pr_info("Debug: buff_seek: %zu\n", buff_seek);
	
	if (copy_to_user(user, kern_info, buff_seek)) {
		pr_alert("writing error.");
		return -EFAULT;
	}
	
	pr_info("info wrote.");
	
	*offset = buff_seek; 
	return buff_seek;
}

static ssize_t proc_file_write(struct file *file, const char __user *user, size_t lenght, loff_t *offset) {
	struct path path;
	int error;
	//char *path_ch = "/home/locci/OS/lab2/test_file.txt";
	char path_ch[BUF_INFO_MAX_SIZE] = {0};

	if (*offset > 0) {
		return 0;
	}
	
	if (copy_from_user(path_ch, user, lenght))
		return -EFAULT;
		
	pr_info("path: %s, path lenght %zu", path_ch, strlen(path_ch));
	
	error = kern_path(path_ch, LOOKUP_FOLLOW, &path);
	if (error) {
		pr_alert("path error.");
		return error;
	}
	inode = path.dentry->d_inode;

	*offset = strlen(path_ch);
	return strlen(path_ch);
}

static int proc_file_open(struct inode *inode, struct file *file){
	pr_info("proc file opened.");
	try_module_get(THIS_MODULE);
	return 0;
}

static int proc_file_close(struct inode *inode, struct file *file){
	pr_info("proc file closed.");
	module_put(THIS_MODULE);
	return 0;
}
