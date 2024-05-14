#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

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
      break;           /* Create a file. */
    case SYS_REMOVE:      
      break;           /* Delete a file. */
    case SYS_OPEN:        
      break;           /* Open a file. */
    case SYS_FILESIZE:    
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
// bool 
// create(const char* file, unsigned initial_size){

// }
// bool 
// remove(const char* file){

// }
// int 
// open(const char* file){

// }
// int 
// filesize(int fd){

// }
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
// void 
// close(int fd){

// }

