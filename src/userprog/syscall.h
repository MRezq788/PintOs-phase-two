#include <stdbool.h>
typedef int pid_t;
#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void halt(void);
void exit(int status);
pid_t exec(const char* cmd_line);
int wait(pid_t pid);
void create(struct intr_frame *f);
void remove(struct intr_frame *f);
void open(struct intr_frame *f);
void filesize(struct intr_frame *f);
void read(int fd, void* buffer, unsigned size);
void write(int fd, const void* buffer, unsigned size);
void seek(int fd, unsigned position);
void tell(int fd);   
void close(struct intr_frame *f);

#endif /* userprog/syscall.h */
