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

#include "mod.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_FILE_NAME "inode_info"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("locci7");
MODULE_DESCRIPTION("Lab 2 module sample.");

static struct proc_dir_entry *proc_file;
static struct inode *inode;

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
	struct inode_info info;
	size_t r_lenght = 0;
	
	pr_info("proc file is reading.");
	
	if (*offset > 0) return 0;
	
	if (inode == NULL) {
		pr_alert("can't find inode from path.");
		return 0;
	}
	else {
		info.i_no = inode->i_ino;
		info.i_count = inode->i_count.counter;
		info.i_mode = inode->i_mode;
		info.i_size = inode->i_size;
		info.m_time = inode->i_mtime.tv_sec;
		
		r_lenght = sizeof(struct inode_info);
	}
	
	if (copy_to_user(user, &info, r_lenght)) 
		return -EFAULT;
	
	pr_info("info wrote.");
	
	*offset = r_lenght; 
	return r_lenght;
}

static ssize_t proc_file_write(struct file *file, const char *user, size_t lenght, loff_t *offset) {

	struct path path;
	int error;
	char *path_ch = kmalloc(lenght, GFP_KERNEL);

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
