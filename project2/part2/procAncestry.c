#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/pid.h>
unsigned long **sys_call_table;

/*asestry info stored here*/
struct ancestry {
  pid_t ancestors[10]; //pid of ancestors
  pid_t siblings[100]; //pid of siblings
  pid_t children[100]; //pid of children
};//end struct

/*save the old version*/
asmlinkage long (*ref_sys_cs3013_syscall2)(void);

asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid,
  struct ancestry *response) {
  struct task_struct *task; // = current; //get pointer to current task_struct
  struct task_struct *iterator;
  //vars stores copies from user space
  unsigned short k_target_pid;
  struct ancestry temp_re; //creat empty struct to avoid NULL Pointer!!
  struct ancestry* k_response = &temp_re;
  //pointers in family tree
  pid_t* ancestors_ptr;
  pid_t* siblings_ptr;
  pid_t* children_ptr;
  pid_t temp;
  struct list_head *siblings_list;
  struct list_head *children_list;

  //copy from user space to kernel
  if(copy_from_user(&k_target_pid, target_pid, sizeof(unsigned short))){
    printk(KERN_INFO "Error: cpy from user: pid\n");
    return EFAULT;
  }
  if(copy_from_user(k_response, response, sizeof(struct ancestry))){
    printk(KERN_INFO "Error: cpy from user: res\n");
    return EFAULT;
  }
  //get pointer to target process
  task = pid_task(find_vpid(k_target_pid), PIDTYPE_PID);
  if(task == NULL){
    printk(KERN_INFO "Error: Cannot obtain PID %d\n", k_target_pid);
    return -ENODEV;
  }else{
    printk(KERN_INFO "Found target PID %d!\n", task->pid);
  }
  //assign values to kernel pointers
  ancestors_ptr = k_response->ancestors;
  siblings_ptr = k_response->siblings;
  children_ptr = k_response->children;

  // printk(KERN_INFO "Done: Load and read\n");
  //find all children of the target
  list_for_each(children_list, &task->children){
    iterator = list_entry(children_list, struct task_struct, sibling);
    temp = iterator->pid;
    *children_ptr++ = temp;
    printk(KERN_INFO "\ttarget %d has child %d\n", task->pid, iterator->pid);
  }

  // printk(KERN_INFO "Done: search children\n");

  //find all siblings of the target
  list_for_each(siblings_list, &task->sibling){
    iterator = list_entry(siblings_list, struct task_struct, sibling);
    if(iterator->pid == 0) {break;} //sibling is ini
    *siblings_ptr++ = iterator->pid;
    printk(KERN_INFO "\ttarget %d has sibling %d\n", task->pid,iterator->pid);
  }

  // printk(KERN_INFO "Done: serach siblings\n");

  //find all parents ofthe target
  iterator = task->parent;
  while(iterator != NULL && iterator->pid != 0){
    *ancestors_ptr++ = iterator->pid;
    printk(KERN_INFO "\ttarget %d has parent %d\n",k_target_pid, iterator->pid);
    iterator = iterator->parent;
  }

  // printk(KERN_INFO "Done: search parents\n");


  //copy from kernel to user space
  if(copy_to_user(response, k_response, sizeof(struct ancestry))){
    printk(KERN_INFO "Error: cpy to user: res\n");
    return EFAULT;
  }

  return 0;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;

  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX\n",
	     (unsigned long) sct);
      return sct;
    }

    offset += sizeof(void *);
  }

  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
  */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work.
       Cancel the module loading step. */
    return -1;
  }

  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;

  enable_page_protection();

  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!\n");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;

  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!\n");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
