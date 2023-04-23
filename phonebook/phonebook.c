#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/printk.h>

#define PHONEBOOK_MAGIC 'p'
#define GET_USER _IOR(PHONEBOOK_MAGIC, 1, char*)
#define ADD_USER _IOW(PHONEBOOK_MAGIC, 2, struct user_data*)
#define DEL_USER _IOW(PHONEBOOK_MAGIC, 3, char*)

#define DEVICE_NAME "phonebook"

struct user_data {
    char name[20];
    char surname[20];
    int age;
    char phone[20];
    char email[50];
};

static int major_number;
static struct class* phonebook_class;

static int phonebook_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}
//static struct device* phonebook_device;

static int phonebook_open(struct inode* inode, struct file* file) 
{
    return 0;
}

static int phonebook_release(struct inode* inode, struct file* file) 
{
    return 0;
}

static ssize_t phonebook_read(struct file* file, char* buffer, size_t length, loff_t* offset){
    return 0;
}

static ssize_t phonebook_write(struct file* file, const char* buffer, size_t length, loff_t* offset)
{
    return 0;
}

#define MAX_PHONEBOOK_SIZE 100

static struct user_data phonebook[MAX_PHONEBOOK_SIZE];
static int phonebook_size = 0;

long my_get_user(const char* surname, unsigned int len, struct user_data* output_data) {
    pr_info("Getting user...\n");
    int i;
    for (i = 0; i < phonebook_size; i++) {
        if (strncmp(surname, phonebook[i].surname, len) == 0) {
            memcpy(output_data, &phonebook[i], sizeof(struct user_data));
            pr_info("User found successfully\n");
            return 0;
        }
    }
    pr_info("User not found\n");
    return -1;
}

long my_add_user(struct user_data* input_data) {
    pr_info("Adding user...\n");
    if (phonebook_size < MAX_PHONEBOOK_SIZE) {
        memcpy(&phonebook[phonebook_size], input_data, sizeof(struct user_data));
        phonebook_size++;
        return 0;
    }
    pr_alert("Reached maximum phonebook size, unable to add new user\n");
    return -1;
}

long my_del_user(const char* surname, unsigned int len)
{
    pr_info("Deleting user...\n");
    int i;
    for (i = 0; i < phonebook_size; i++) {
        if (strncmp(surname, phonebook[i].surname, len) == 0) {
            memmove(&phonebook[i], &phonebook[i+1], (phonebook_size-i-1)*sizeof(struct user_data));
            phonebook_size--;
            return 0;
        }
    }
    pr_alert("User doesn't exist\n");
    return -1;
}

static long phonebook_ioctl(struct file* file, unsigned int cmd, unsigned long arg) 
{
    switch (cmd) {
        case GET_USER:
            return my_get_user((const char*)arg, strlen((const char*)arg), (struct user_data*)arg);
        case ADD_USER:
            return my_add_user((struct user_data*)arg);
        case DEL_USER:
            return my_del_user((const char*)arg, strlen((const char*)arg));
        default:
            return -EINVAL;
    }
}

static struct file_operations phonebook_fops = {
    .owner = THIS_MODULE,
    .open = phonebook_open,
    .release = phonebook_release,
    .read = phonebook_read,
    .write = phonebook_write,
    .unlocked_ioctl = phonebook_ioctl,
};

static int __init phonebook_init(void) 
{
    major_number = register_chrdev(0, DEVICE_NAME, &phonebook_fops);

    if (major_number < 0) {
        pr_alert("Registering char device failed with %d\n", major_number);
        return major_number;
    }

    pr_info("I was assigned major number %d.\n", major_number);

    phonebook_class = class_create(THIS_MODULE, DEVICE_NAME);
    phonebook_class->dev_uevent = phonebook_uevent;
    device_create(phonebook_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit phonebook_exit(void) {
    device_destroy(phonebook_class, MKDEV(major_number, 0));
    class_unregister(phonebook_class);
    class_destroy(phonebook_class);
    unregister_chrdev(major_number, DEVICE_NAME);
}

module_init(phonebook_init);
module_exit(phonebook_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Egor Burdin");
MODULE_DESCRIPTION("Phonebook module");
