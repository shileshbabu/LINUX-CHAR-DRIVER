#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "test1"

struct pca9500_chip {
	struct i2c_client	*client;
	struct i2c_msg 	xfer_msg[2];
	struct mutex 		xfer_lock;
};

static int major;
static char internal_buffer[3] ;
struct pca9500_chip chip;

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

static ssize_t fops_read(struct file *filp, char *buffer, size_t count,loff_t *offset)
{
	s32 data;
	
	struct i2c_client *client = chip.client;
	
	if (count < 3)
	{
		printk(KERN_INFO "read error\n");
		return 0;
	}

	printk(KERN_INFO"read device\n");
	printk(KERN_INFO"  read buffer address = %x\n",buffer); 

	 data = i2c_smbus_read_byte_data(client, 0xEE);
	copy_to_user( buffer, &data, 4); 
	
	return count;
}

static ssize_t fops_write(struct file *filp,const char __user *buffer, size_t count, loff_t *offset)
{	
	int led;
	int val;
	int led_val;
	int mask = ((1<< 4) | (1<< 0));
	u8 value[2];
	int ret;
	
	struct i2c_msg *msg = &chip.xfer_msg[0];
	struct i2c_client *client = chip.client;

	if (count < 3)
	{
		printk(KERN_INFO "read error\n");
		return 0;
	}

	copy_from_user(internal_buffer, buffer, count);

	printk(KERN_INFO" write buffer address  = %x\n",buffer); 

	printk(KERN_INFO" write buffer[0]  = %d\n",(buffer[0]-'0')); 
	printk(KERN_INFO" write buffer[1]  = %d\n",(buffer[1]-'0')); 
	printk(KERN_INFO" write buffer[2]  = %d\n",(buffer[2]-'0')); 
	
	if((buffer[0]-'0') == 0)
	{
		led = 	(buffer[1]-'0');
		printk(KERN_INFO" led  = %d\n",led); 
		printk(KERN_INFO" val  = %d\n",(buffer[2]-'0'));
	}
	else
	{
		led = (buffer[0]-'0')*10 + (buffer[1]-'0');
		printk(KERN_INFO" led  = %d\n",led); 
		printk(KERN_INFO" val  = %d\n",(buffer[2]-'0'));
	}

	val = 	(buffer[2]-'0');

	if(led)
	{
		mask = mask << 1;
	}

	if(val == 0)
	{
		led_val = led_val & ~(mask);
	}
	else
	{
		led_val = led_val |(mask);
	}
	
	printk(KERN_INFO "%x %x \n", client->addr, led_val);  
	mutex_lock(&chip.xfer_lock);

#if 	1
	i2c_smbus_write_byte_data(client, 0xEE, led_val);
#else
	msg->addr 	= 0x4A;
	msg->len	= 2;
	msg->flags 	= 0;
	msg->buf 	= value;

	value[0] = 0xEE;
	value[1] = led_val;

	ret = i2c_transfer(client->adapter, msg, 1);
#endif

	mutex_unlock(&chip.xfer_lock);

	printk("ret = %d\n",ret);	

	return count;																					 
}

static struct file_operations fops =
{
	.read 	= fops_read,
	.write   	= fops_write,
	.open    	= fops_open,
	.release 	= fops_release
};

static int pca9554_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int ret;

	client->addr = 0x4A;
	chip.client = client;

	i2c_set_clientdata(client, &chip);
	mutex_init(&chip.xfer_lock);
	
	printk("I2c's probe call..........................\n");
	major = register_chrdev(0, DEVICE_NAME, &fops);
	strcpy(internal_buffer,"lexistech_ghandhinagar");
	printk (KERN_INFO "Registering char device major no is ::%d\n", major);
	return 0;
}

static int pca9554_remove(struct i2c_client *client)
{	
	unregister_chrdev(major, DEVICE_NAME);	
	return 0;
}

static const struct i2c_device_id pca9554_id[] = {
	{ "pca9554_lexis", 0 },									/*pca9554 is device name*/
	{ }
};
MODULE_DEVICE_TABLE(i2c, pca9554_id);

static struct i2c_driver pca9554_driver_lexis = {
	.driver		= {
		.name	= "lexis",								/*importent line*/
	},
	.probe		= pca9554_probe,
	.remove		= pca9554_remove,
	.id_table	= pca9554_id,							/*importent line*/
};

static int __init pca9554_init(void)
{	
	printk("i2c_init_call....................................................................................................\n");
	return i2c_add_driver(&pca9554_driver_lexis);
}

static void __exit pca9554_exit(void)
{
	i2c_del_driver(&pca9554_driver_lexis);
}

MODULE_AUTHOR("Alessandro Zummo <a.zummo@towertech.it>");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(pca9554_init);
module_exit(pca9554_exit);

