#define __LIBC_SRC_
#include "libc.h"

// ============== Syscall Wrappers ==============

static long syscall3(long num, long arg1, long arg2, long arg3)
{
    int result;
    asm volatile (
        "int $0x80"
        : "=a" (result)
        : "0" (num), "b" (arg1), "c" (arg2), "d" (arg3)
        : "memory"
    );
    return result;
}

__attribute((noreturn)) void exit(int r)
{
    syscall3(SYS_exit, r, 0, 0);
    __builtin_unreachable();
}

pid_t fork(void) 
{
    return (pid_t) syscall3(SYS_fork, 0, 0, 0);
}

ssize_t read(int fd, void *buf, size_t count) 
{
    return (ssize_t) syscall3(SYS_read, fd, (long) buf, count);
}

ssize_t write(int fd, const void *buf, size_t count) 
{
    return (ssize_t) syscall3(SYS_write, fd, (long) buf, count);
}

int open(const char *pathname, int flags, int mode) {
    return syscall3(SYS_open, (long) pathname, flags, mode);
}

int close(int fd) 
{
    return syscall3(SYS_close, fd, 0, 0);
}

pid_t waitpid(pid_t pid, int *status, int options) 
{
    return (pid_t) syscall3(SYS_waitpid, pid, (long) status, options);
}

int execve(const char *pathname, char *const argv[], char *const envp[]) 
{
    return syscall3(SYS_execve, (long) pathname, (long) argv, (long) envp);
}

int pipe(int pipefd[2]) 
{
    return syscall3(SYS_pipe, (long) pipefd, 0, 0);
}

int dup2(int oldfd, int newfd) 
{
    return syscall3(SYS_dup2, oldfd, newfd, 0);
}


asm(
    ".global mmap2\n"
    "mmap2:\n"
    "    push %ebp\n"
    "    push %ebx\n"
    "    mov 0xc(%esp), %ebx\n"
    "    mov 0x10(%esp), %ecx\n"
    "    mov 0x14(%esp), %edx\n"
    "    mov 0x18(%esp), %esi\n"
    "    mov 0x1c(%esp), %edi\n"
    "    mov 0x20(%esp), %ebp\n"
    "    mov $192, %eax\n"
    "    int $0x80\n"
    "    pop %ebx\n"
    "    pop %ebp\n"
    "    ret\n"
);

extern void *mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

// ============== String Functions ==============

size_t strlen(const char *s) 
{
    size_t len = 0;
    while (*s++){
        len++;
    }
    return len;
}

void *memcpy(void *dest, const void *src, size_t len)
{
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

static void *mempcpy(void *dst, const void *src, size_t len)
{
  return (char *) memcpy (dst, src, len) + len;
}

static char *stpcpy(char *restrict dst, const char *restrict src)
{
    char  *p;

    p = mempcpy(dst, src, strlen(src));
    *p = '\0';

    return p;
}

char *strcat(char *restrict dst, const char *restrict src)
{
    stpcpy(dst + strlen(dst), src);
    return dst;
}

int atoi(const char *nptr)
{
    int sign = 1, res = 0, idx = 0;

    while (nptr[idx] == ' ')
        idx++;
    
    if (nptr[idx] == '-' || nptr[idx] == '+') {
        sign = (nptr[idx++] == '-') ? -1 : 1;
    }

    while (nptr[idx] >= '0' && nptr[idx] <= '9') {
        if (res > INT_MAX / 10 || (res == INT_MAX / 10 && nptr[idx] - '0' > 7)) {
            return sign == 1 ? INT_MAX : INT_MIN;
        }
      
        res = 10 * res + (nptr[idx++] - '0');
    }

    return res * sign;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(unsigned char *) s1 - *(unsigned char *) s2;
}

static void swap(char *a, char *b)                                                                                                                                                                       
{
    if(!a || !b)
        return;

    char temp = *(a);
    *(a) = *(b);
    *(b) = temp;
}

static void reverse(char *str, int length) 
{ 
	int start = 0; 
	int end = length -1; 
	while (start < end) { 
		swap((str+start), (str+end)); 
		start++; 
		end--; 
	} 
} 

char* itoa(int num, char* str, int base) 
{ 
	int i = 0; 
	int is_neg = 0; 

	if (num == 0) { 
		str[i++] = '0'; 
		str[i] = '\0'; 
		return str; 
	}

	if (num < 0 && base == 10) { 
		is_neg = 1;
		num = -num; 
	} 

	while (num != 0) { 
		int rem = num % base; 
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
		num = num/base; 
	}

	if (is_neg) 
		str[i++] = '-'; 

	str[i] = '\0';
	reverse(str, i); 
	return str; 
} 

// ================== I/O ==================

int puts(const char *s)
{
    int len = strlen(s);
    if (write(1, s, len) != len) {
        return -1;
    }
    if (write(1, "\n", 1) != 1) {
        return -1;
    }
    return 0;
}

// ============== Utility Functions ==============

int popen(char *command)
{
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        close(pipefd[1]);
        execve("/bin/sh", (char *[]) {"/bin/sh", "-c", command, NULL}, NULL);
        exit(0);
    }

    waitpid(-1, NULL, 0);
    close(pipefd[1]);
    return pipefd[0];
}