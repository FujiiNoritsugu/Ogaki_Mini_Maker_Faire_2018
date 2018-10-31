#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/spinlock_types.h>

#define DEVNAME "test_int"
#define INPUT_BASE 0xc0000000
#define INPUT_SIZE 64 * PAGE_SIZE
#define OUTPUT_BASE 0xff200000
#define OUTPUT_SIZE PAGE_SIZE
#define DATA_SIZE 1000

void *sensor_mem;
void *servo_mem;

static DEFINE_SEMAPHORE(interrupt_mutex);
static DECLARE_WAIT_QUEUE_HEAD(interrupt_wq);

static int interrupt_flag = 0;
static DEFINE_SPINLOCK(interrupt_flag_lock);

int sensor_data[DATA_SIZE] = {0};

MODULE_LICENSE("Dual BSD/GPL");

static irq_handler_t __test_isr(int irq, void *dev_id, struct pt_regs *regs){
	printk(KERN_INFO DEVNAME ": ISR \n");
	spin_lock(&interrupt_flag_lock);
	interrupt_flag = 1;

	// ここでsensor_memにセンサデータをコピーする
	memcpy(sensor_data, sensor_mem, sizeof(u_int) * DATA_SIZE); 
	spin_unlock(&interrupt_flag_lock);

	wake_up_interruptible(&interrupt_wq);
	return (irq_handler_t) IRQ_HANDLED;
}

static int __test_int_driver_probe(struct platform_device * pdev){
	int irq_num;
	int res;
	irq_num = platform_get_irq(pdev, 0);

	printk(KERN_INFO DEVNAME ": IRQ %d about to be registered !\n", irq_num);

	res = request_irq(irq_num, (irq_handler_t)__test_isr, 0, DEVNAME, NULL);

	return res;
}

static int __test_int_driver_remove(struct platform_device *pdev){
	int irq_num;
	irq_num = platform_get_irq(pdev, 0);

	printk(KERN_INFO "test_int: IRQ %d about to be freed!\n", irq_num);

	free_irq(irq_num, NULL);

	return 0;
}

static const struct of_device_id __test_int_driver_id[] = {
	{.compatible = "memory_socket,memory_socket-1.0"},
	{}
};

static struct platform_driver __test_int_driver = {
	.driver = {
		.name = DEVNAME,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(__test_int_driver_id),
		.bus = &platform_bus_type,
	},
	.probe = __test_int_driver_probe,
	.remove = __test_int_driver_remove
};

static ssize_t __test_int_driver_show(struct device_driver *drv, char *buf)
{
	int res;
	char *str = buf;

	if(down_trylock(&interrupt_mutex)){
		return -EAGAIN;
	}

	if(wait_event_interruptible(interrupt_wq, interrupt_flag != 0)){
		return -ERESTART;
		goto release_and_exit;
	}

	spin_lock(&interrupt_flag_lock);
	interrupt_flag = 0;
	spin_unlock(&interrupt_flag_lock);

	memcpy(str,sensor_data,sizeof(u_int) * DATA_SIZE);

	res = sizeof(int) * DATA_SIZE;

release_and_exit:
	up(&interrupt_mutex);
	return res;
}

static ssize_t __test_int_driver_store(struct device_driver *drv, const char *buf, size_t count)
{
	u16 width;
	u8 width1, width2;
	
	if(buf == NULL){
		pr_err("Error string must not be NULL\n");
		return -EINVAL;
	}

	if(kstrtou16(buf, 10, &width) < 0){
		pr_err("Could not convert string to integer\n");
		return -EINVAL;
	}

	if(width < 0 || width > 65535){
		pr_err("width < 0 or width > 65535");
		return -EINVAL;
	}

	width1 = 0x00ff & width;
	width2 = ((0xff00 & width) >> 8);

printk("buf = %s width = %d width1 = %d, width2 = %d \n", buf, width, width1, width2);

	iowrite8(width1, servo_mem);
	iowrite8(width2, servo_mem+1);

	return count;
}

static DRIVER_ATTR(__test_int_driver, S_IRUSR | S_IWUSR, __test_int_driver_show, __test_int_driver_store);
 
static int __init __test_int_driver_init(void)
{
	int res;
	struct resource *reso;

	res = platform_driver_register(&__test_int_driver);
        if (res < 0){
printk("driver_register fail");
                goto fail_driver_register;
        }

	res = driver_create_file(&__test_int_driver.driver, &driver_attr___test_int_driver);
	if(res < 0){
		goto fail_driver_create_file;
	}

	reso = request_mem_region(INPUT_BASE, INPUT_SIZE, "sensor_input");
	if(reso == NULL){
		res = -EBUSY;
		goto fail_request_mem;
	}

	sensor_mem = ioremap(INPUT_BASE, INPUT_SIZE);
	if(sensor_mem == NULL){
		res = -EFAULT;
		goto fail_ioremap;
	}

        reso = request_mem_region(OUTPUT_BASE, OUTPUT_SIZE, "servo_output");
        if(reso == NULL){
                res = -EBUSY;
                goto fail_request_mem_out;
        }

        servo_mem = ioremap(OUTPUT_BASE, OUTPUT_SIZE);
        if(servo_mem == NULL){
                res = -EFAULT;
                goto fail_ioremap_out;
        }

	return res;
fail_ioremap_out:
        iounmap(servo_mem);
fail_request_mem_out:
        release_mem_region(OUTPUT_BASE, OUTPUT_SIZE);
fail_ioremap:
	iounmap(sensor_mem);
fail_request_mem:
	release_mem_region(INPUT_BASE, INPUT_SIZE);
fail_driver_create_file:
	platform_driver_unregister(&__test_int_driver);
fail_driver_register:
        return res;
}

static void __exit __test_int_driver_exit(void){
        iounmap(servo_mem);
        release_mem_region(OUTPUT_BASE, OUTPUT_SIZE);
	iounmap(sensor_mem);
	release_mem_region(INPUT_BASE, INPUT_SIZE);
	driver_remove_file(&__test_int_driver.driver, &driver_attr___test_int_driver);
	platform_driver_unregister(&__test_int_driver);
}

module_init(__test_int_driver_init);
module_exit(__test_int_driver_exit);
