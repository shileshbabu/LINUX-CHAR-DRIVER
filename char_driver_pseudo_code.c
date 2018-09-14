#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>		
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "test1"
static int major;
char* internal_buffer;
int internal_buffer_size = 100;

#define MYDRIVER_IOC_MAGIC 'Z'
#define MYDRIVER_GET_SIZE		_IOR(MYDRIVER_IOC_MAGIC, 0, int)
#define MYDRIVER_SET_SIZE 		_IOW(MYDRIVER_IOC_MAGIC, 1, int)

static int fops_open(struct inode *node, struct file *filp)
{
	printk(KERN_INFO "open device\n");						
	return 0;												
}

static int fops_release(struct inode *node, struct file *filp)
{
	printk(KERN_INFO"release device\n");

	return 0;
}

static ssize_t  fops_read(struct file *fl,char *buffer,size_t count,loff_t *offset)
{	
	if(count > internal_buffer_size)
	{
		count = internal_buffer_size;
	}

	printk(KERN_INFO"read device\n");
	copy_to_user( buffer, internal_buffer, count); 	
	return count;
}

static ssize_t fops_write(struct file *fl,const char __user *buffer,size_t count,loff_t *offset)
{
 
	if(count > internal_buffer_size)
	{
		count = internal_buffer_size;
	}
	copy_from_user(internal_buffer, buffer, count);
	printk(KERN_INFO" write buffer  = %s\n",buffer); 
	internal_buffer[count] = '\0';

	return count;
}

long fops_ioctl(struct file * filep, unsigned int ioctl_num, unsigned long ioctl_param)
{

	switch (ioctl_num) 
	{
		case MYDRIVER_GET_SIZE:
		{
			printk(KERN_INFO "Call ioctl Get size");
			break;
		}
		case MYDRIVER_SET_SIZE:
		{
			printk(KERN_INFO "Call ioctl Set size");
			break;
		}
	}

	return 0;
}

static int kfd_mmap(struct file *filp, struct vm_area_struct *vma)
{
	/*mmap is used to request a mapping of device memory to a process's address space. 
	If this method is NULL, the mmap system call returns -ENODEV.*/
	
	return 0;
}

loff_t device_lseek(struct file *file, loff_t offset, int orig) 
{
	/*llseek method is used to change the current read/write position in a file*/
	return 0;

}

unsigned int file_pole(struct file *file, struct poll_table_struct *poll_table)
{
#if 0
The poll method is the back end of three system calls: poll, epoll, and select, all of which are used to query whether a read or write to one or more file descriptors would block. The poll method should return a bit mask indicating whether non-blocking reads or writes are possible, and, possibly, provide the kernel with information that can be used to put the calling process to sleep until I/O becomes possible. If a driver leaves its poll method NULL, the device is assumed to be both readable and writable without blocking.
#endif
}

int file_flush(struct file *file)
{
#if 0
The flush operation is invoked when a process closes its copy of a file descriptor for a device; it should execute (and wait for) any outstanding operations on the device. This must not be confused with the fsync operation requested by user programs. Currently, flush is used in very few drivers; the SCSI tape driver uses it, for example, to ensure that all data written makes it to the tape before the device is closed. If flush is NULL, the kernel simply ignores the user application request.
#endif
}

static int f_fsync(struct file *file, loff_t a, loff_t b, int datasync)			//comment for a variable
{

}

static struct file_operations fops=
{	
	.open 			= fops_open,
	.release 		= fops_release,
	.read 			= fops_read,
	.write 			= fops_write,
	.unlocked_ioctl 	= fops_ioctl,
	.mmap 			= kfd_mmap,
	.llseek 		= device_lseek,
	.poll			= file_pole,
	.flush			= file_flush,
	.fsync			= f_fsync,
};

static int __init start(void)									
{
	internal_buffer = kmalloc(internal_buffer_size, GFP_KERNEL);
	if(internal_buffer == NULL)
	{
		printk("Eroor while allocating memory\n");
		return -1;
	}

	major = register_chrdev(0, DEVICE_NAME, &fops);

	printk(KERN_INFO "init module call \n");
   
	printk (KERN_INFO "Registering char device major no is ::%d\n", major);
	strcpy(internal_buffer,"Shilesh");

	return 0;
}
static void __exit close(void)									
{
	if(internal_buffer != NULL)
	{
		kfree(internal_buffer);
	}
	
	printk(KERN_ALERT "exit module call\n");
	unregister_chrdev(major, DEVICE_NAME);				
}

module_init(start);												
module_exit(close);												
