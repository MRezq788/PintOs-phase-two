#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
#include "threads/synch.h"
#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <threads/vaddr.h>
#include <userprog/pagedir.h>
#include <filesys/directory.h>
#include <filesys/file.h>
#include <filesys/filesys.h>

static void syscall_handler (struct intr_frame *);
static struct lock files_sync_lock;
void validate_void_pointer (const void* pt);
char* get_char_ptr(char*** esp, int offset);
void* get_void_ptr(void*** esp);
unsigned get_unsigned(int** esp, int offset);
int get_int(int** esp, int offset);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
     lock_init(&files_sync_lock);
}
// function to validate that the address provided by the user is not null and inside my user space
// and it's mapped in the page table (have a physical address)


static static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  //first check if f->esp is a valid pointer)
  // if (f->esp is a bad pointer)
  //   exit(-1);

//cast f->esp into an int*, then dereference it for the SYS_CODE
  switch(*(int*)f->esp)
  {
    case SYS_HALT: 
      halt(f);       
      break;           /* Halt the operating system. */
    case SYS_EXIT:
      exit(f);   
      break;           /* Terminate this process. */
    case SYS_EXEC:        
      break;           /* Start another process. */
    case SYS_WAIT:
      void *esp = f->esp;
      int pid = (*((int *)esp + 1));
      f->eax = wait(pid);     
      break;           /* Wait for a child process to die. */
    case SYS_CREATE:      
      create(f);
      break;           /* Create a file. */
    case SYS_REMOVE:
      remove(f);
      break;           /* Delete a file. */
    case SYS_OPEN:   
      open(f);
      break;           /* Open a file. */
    case SYS_FILESIZE: 
      filesize(f);
      break;           /* Obtain a file's size. */
    case SYS_READ:
      read(f);        
      break;           /* Read from a file. */
    case SYS_WRITE:
      write(f);       
      break;           /* Write to a file. */
    case SYS_SEEK:
      seek(f);        
      break;           /* Change position in a file. */
    case SYS_TELL:
      tell(f);        
      break;           /* Report current position in a file. */
    case SYS_CLOSE: 
      close(f);
      break;           /* Close a file. */
  }
  thread_exit ();
}

int wait(int pid)
{
  return process_wait(pid);
}

int 
get_int(int** esp, int offset){
  return *((int*)esp + offset);
}
unsigned 
get_unsigned(int** esp, int offset){
  return *((unsigned*)esp + offset);
}
char*
get_char_ptr(char*** esp, int offset){
  return (char*)(*((int*)esp + offset));
}
void*
get_void_ptr(void*** esp){
  return (void*)(*((int*)esp + 2));
}
void validate_void_pointer(const void *pt)
{
    if (is_kernel_vaddr(pt) || pt==NULL || pagedir_get_page(thread_current()->pagedir, pt)==NULL) {
      //exit(-1);
      printf("exit\n");
    }
}

void 
halt(struct intr_frame *f){
   f->eax = shutdown_power_off();
}
void 
exit(struct intr_frame *f){
  //pull the arguments
  int status = get_int(*(int*)f->esp, 1);

  struct thread* cur = thread_current();
  struct thread *curr = thread_current()->parent;
  if (curr){
    curr->childState = status;
  }
  //save exit status at process descriptor
  cur->status = status;
  //do the functionality
  printf("%s: exit(%d)\n", cur->name, status);

  for (int fd_index = 2; fd_index < 64; fd_index++)
    file_close(cur->fdt[fd_index]);
  
  thread_exit();
}
// pid_t
//  exec(const char* cmd_line){

// }
// int 
// wait(pid_t pid){

// }

void 
create(struct intr_frame *f) {
  // pull the arguments
  unsigned initial_size = get_unsigned(&f->esp, 1);
  char* file = get_char_ptr(&f->esp,2);

  //validate pointers
  validate_void_pointer((void *)file);

  //do the functionality
  f->eax = filesys_create(file, initial_size);
}

void 
remove(struct intr_frame *f) {
  //pull the arguments
  char* file_name = get_char_ptr(&f->esp,1);

  //validate pointers
  validate_void_pointer((void*)file_name);

  //do the functionality
  f->eax = filesys_remove(file_name);
}

void 
open(struct intr_frame *f) {
  //pull the arguments
  char* file_name = get_char_ptr(&f->esp,1);

  //validate pointers
  validate_void_pointer((void*)file_name);

  //do the functionality
  struct file* file = filesys_open(file_name);
  if (file==NULL) {
    f->eax = -1;
  }
  else {
    int next_fd_index = thread_current()->next_fd++;
    thread_current()->fdt[next_fd_index] = file;
    f->eax = next_fd_index;
  }
}

void filesize(struct intr_frame *f) {
  //pull the arguments
  int fd_index = get_int(&f->esp,1);

  //do the functionality
  struct file* file = thread_current()->fdt[fd_index];
  if (file!=NULL) {
    f->eax = file_length(file);
  }
  else {
    f->eax = NULL;
  }  
}

void 
read(struct intr_frame *f){
  //pull the arguments
  int fd_index = get_int(&f->esp, 1);
  void* buffer = get_void_ptr(&f->esp);
  unsigned size = get_unsigned(&f->esp, 3);

  //validate pointers
  validate_void_pointer((void *)buffer);
  
  //do the functionality
  if (fd_index==0)
    f->eax = input_getc();
  else
    f->eax = file_read(thread_current()->fdt[fd_index], buffer, size);
}
void 
write(struct intr_frame *f){
  //pull the arguments
  int fd_index = get_int(&f->esp, 1);
  void* buffer = get_void_ptr(&f->esp);
  unsigned size = get_unsigned(&f->esp, 3);

  //validate pointers
  validate_void_pointer((void *)buffer);
  
  //do the functionality
  if (fd_index==1)
    f->eax = putbuf(buffer, size);
  else
    f->eax = file_write(thread_current()->fdt[fd_index], buffer, size);
}

void 
seek(struct intr_frame *f){
  //pull the arguments
  int fd_index = get_int(&f->esp, 1);
  unsigned position = get_unsigned(&f->esp, 2);
  
  //do the functionality
  file_seek(thread_current()->fdt[fd_index], position);
}

void 
tell(struct intr_frame *f){
  //pull the arguments
  int fd_index = get_int(&f->esp, 1);
  
  //do the functionality
  f->eax = file_tell(thread_current()->fdt[fd_index]);
}

void 
close(struct intr_frame *f) {
  //pull the arguments
  int fd_index = get_int(&f->esp,1);

  //do the functionality
  file_close(thread_current()->fdt[fd_index]);
}
