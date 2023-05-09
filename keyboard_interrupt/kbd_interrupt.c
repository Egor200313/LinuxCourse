#include <linux/interrupt.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static const unsigned int KEYBOARDIRQ = 1;
static atomic_t cnt = ATOMIC_INIT(0);
static const unsigned int SECS_TO_SLEEP = 60;

static struct tasklet_struct task;
struct task_struct* writer;

void cnt_incrementor(unsigned long data) {
    atomic_inc(&cnt);
}

static irqreturn_t interrupt_handler(int irq, void* dev_id) {
    tasklet_schedule(&task);
    return IRQ_HANDLED;
}

int writer_routine(void* data) {
    unsigned int counter;

    while(!kthread_should_stop()) {
        ssleep(SECS_TO_SLEEP);
        counter = atomic_xchg(&cnt, 0);
        pr_info("Keyboard pressed %u times\n", counter);
    }
    return counter;
}

static int __init kbd_interrupt_init(void) {
    pr_info("Keyboard interrupter inserted");
    int status;
    if ((status = request_irq(KEYBOARDIRQ, interrupt_handler, IRQF_SHARED, "kbd_interrupt", &task)) < 0) {
        pr_info("Unable to init interrupt handler");
        return status;
    }
    tasklet_init(&task, cnt_incrementor, 0);
    writer = kthread_run(writer_routine, NULL, "kbd_printer_job");

    return 0;
}

static void __exit kbd_interrupt_exit(void) {
    pr_info("Keyboard interrupter removed");
    kthread_stop(writer);
    tasklet_kill(&task);
    free_irq(KEYBOARDIRQ, &task);
}

module_init(kbd_interrupt_init);
module_exit(kbd_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Egor Burdin");
MODULE_DESCRIPTION("Keyboard interrupt module");
