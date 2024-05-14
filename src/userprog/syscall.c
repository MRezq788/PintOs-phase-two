#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <sys/types.h>
#include <syscall.h>

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}
char* get_char_ptr(char*** esp);
void* get_void_ptr(void*** esp);
unsigned get_unsigned(int** esp, int offset);
int get_int(int** esp, int offset);

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

void 
syscall_init (void){

}
void 
halt(void){

}
void 
exit(int status){

}
pid_t
 exec(const char* cmd_line){

}
int 
wait(pid_t pid){

}
bool 
create(const char* file, unsigned initial_size){

}
bool 
remove(const char* file){

}
int 
open(const char* file){

}
int 
filesize(int fd){

}
int 
read(int fd, void* buffer, unsigned size){

}
int 
write(int fd, const void* buffer, unsigned size){

}
void 
seek(int fd, unsigned position){

}
unsigned 
tell(int fd){

}
void 
close(int fd){

}

