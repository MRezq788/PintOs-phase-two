#include <stdbool.h>
typedef int pid_t;
#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void halt(void);
void exit(int status);
pid_t exec(const char* cmd_line);
int wait(pid_t pid);
bool create(struct intr_frame *f);
bool remove(struct intr_frame *f);
int open(struct intr_frame *f);
int filesize(struct intr_frame *f);
int read(int fd, void* buffer, unsigned size);
int write(int fd, const void* buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(struct intr_frame *f);

#endif /* userprog/syscall.h */
