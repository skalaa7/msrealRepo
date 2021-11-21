#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#define BUFF_SIZE 20

MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

int stek[16];
int pos = 0;


int stek_open(struct inode *pinode, struct file *pfile);
int stek_close(struct inode *pinode, struct file *pfile);
ssize_t stek_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stek_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stek_open,
	.read = stek_read,
	.write = stek_write,
	.release = stek_close,
};


int stek_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened file\n");
		return 0;
}

int stek_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed file\n");
		return 0;
}

ssize_t stek_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	long int len;
	while((stek[pos]==0)&&(pos>0)){
                pos--;

	}
	if((stek[pos]==0)&&(pos==0)){
			printk(KERN_INFO "Stack is empty");
			}

		
	len = scnprintf(buff,BUFF_SIZE , "%d ", stek[pos]);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;
	else{
		printk(KERN_INFO "%d\n",stek[pos]);
		printk(KERN_INFO "Succesfully read from file from position %d\n",pos);
		stek[pos]=0;
		
		return 0;
	}
	
	if (pos >15) {
		pos=15;
		printk(KERN_INFO "Stack is full");
	}
	else if(pos<0)
	{
		printk(KERN_INFO "Stack is empty");
		pos=0;
	
	}
	
	return len;
}

ssize_t stek_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	int  value;
	int ret;
        
	while(stek[pos]!=0){
		pos++;
	}
	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

	ret = sscanf(buff,"%d",&value);

	if(ret==1)//two parameters parsed in sscanf
	{
		if(pos >=0 && pos <=15)
		{
			stek[pos] = value;
			printk(KERN_INFO "Succesfully wrote value %d in position %d\n", value, pos);
		       	
		}
		else
		{
			printk(KERN_WARNING "Position should be between 0 and 16\n"); 
		}
	}
	else
	{
		printk(KERN_WARNING "Wrong command format\nexpected: n,m\n\tn-position\n\tm-value\n");
	}
	if (pos >15) {
		pos=15;
		printk(KERN_INFO "Stack is full");
	}
	else if(pos<0)
	{
		printk(KERN_INFO "Stack is empty");
		pos=0;
	
	}

	return length;
}

static int __init stek_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<16; i++)
		stek[i] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stek");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stek_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stek");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stek_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stek_init);
module_exit(stek_exit);
