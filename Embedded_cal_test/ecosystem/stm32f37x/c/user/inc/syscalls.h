/// Minimal system call implementation for newlib
/// Modified and removed from Neel Shaw's original implementation of mUSB.c.

#ifdef SYSCALLS_H
#define SYSCALLS_H

#ifdef __cplusplus
extern "C" {
#endif

int _write(int file, char *ptr, int len);
caddr_t _sbrk(int incr);
int _read(int file, __IO char *ptr, int len);
int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SYSCALLS_H
