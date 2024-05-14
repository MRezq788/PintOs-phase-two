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
char* get_char_ptr(char*** esp);
void* get_void_ptr(void*** esp);
unsigned get_unsigned(int** esp, int offset);
int get_int(int** esp, int offset);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(files_sync_lock);
}

static void
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
      break;           /* Halt the operating system. */
    case SYS_EXIT:
      exit(get_int(*(int*)f->esp, 1));   
      break;           /* Terminate this process. */
    case SYS_EXEC:        
      break;           /* Start another process. */
    case SYS_WAIT:        
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
      break;           /* Read from a file. */
    case SYS_WRITE:       
      break;           /* Write to a file. */
    case SYS_SEEK:        
      break;           /* Change position in a file. */
    case SYS_TELL:        
      break;           /* Report current position in a file. */
    case SYS_CLOSE: 
      close(f);
      break;           /* Close a file. */
  }
  thread_exit ();
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
get_char_ptr(char*** esp){
  return (char*)(*((int*)esp + 1));
}
void*
get_void_ptr(void*** esp){
  return (void*)(*((int*)esp + 2));
}

// void 
// syscall_init (void){

// }
// void 
// halt(void){

// }
void 
exit(int status){
  struct thread* cur = thread_current();
  //save exit status at process descriptor
  cur->status = status;
  printf("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}
// pid_t
//  exec(const char* cmd_line){

// }
// int 
// wait(pid_t pid){

// }

bool create(struct intr_frame *f) {
  unsigned initial_size = get_unsigned(&f->esp, 1);
  char* file = get_char_ptr(&f->esp,2);
  return filesys_create(file, initial_size);
}

bool remove(struct intr_frame *f) {
  char* file_name = get_char_ptr(&f->esp,1);
  return filesys_remove(file_name);
}

int open(struct intr_frame *f) {
  char* file_name = get_char_ptr(&f->esp,1);
  struct file* file = filesys_open(file_name);
  if (file==NULL) {
    return -1;
  }
  else {
    int next_fd_index = thread_current()->next_fd++;
    thread_current()->fdt[next_fd_index] = file;
    return next_fd_index;
  }
}

int filesize(struct intr_frame *f) {
  int fd_index = get_int(&f->esp,1);
  return file_length(thread_current()->fdt[fd_index]);
}

int 
read(int fd, void* buffer, unsigned size){
  if (fd==0)
    return input_getc();
  else
    return file_read(fd, buffer, size);
}
int 
write(int fd, const void* buffer, unsigned size){
  if (fd==1)
    return putbuf(buffer, size);
  else
    return file_write(fd, buffer, size);
}
void 
seek(int fd, unsigned position){
  file_seek(fd, position);
}
unsigned 
tell(int fd){
  return file_tell(fd);
}

void close(struct intr_frame *f) {
  int fd_index = get_int(&f->esp,1);
  file_close(thread_current()->fdt[fd_index]);
}

