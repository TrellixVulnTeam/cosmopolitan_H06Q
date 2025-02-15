#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _timespec_zero ((struct timespec){0})
#define _timespec_max  ((struct timespec){0x7fffffffffffffff, 999999999})

struct timespec {
  int64_t tv_sec;
  int64_t tv_nsec; /* nanoseconds */
};

int clock_getres(int, struct timespec *);
int clock_gettime(int, struct timespec *);
int clock_nanosleep(int, int, const struct timespec *, struct timespec *);
int futimens(int, const struct timespec[2]);
int nanosleep(const struct timespec *, struct timespec *);
int sys_futex(int *, int, int, const struct timespec *, int *);
int utimensat(int, const char *, const struct timespec[2], int);
int timespec_get(struct timespec *, int);
int timespec_getres(struct timespec *, int);

int _timespec_cmp(struct timespec, struct timespec) pureconst;
bool _timespec_eq(struct timespec, struct timespec) pureconst;
bool _timespec_gt(struct timespec, struct timespec) pureconst;
bool _timespec_gte(struct timespec, struct timespec) pureconst;
int64_t _timespec_tomicros(struct timespec) pureconst;
int64_t _timespec_tomillis(struct timespec) pureconst;
int64_t _timespec_tonanos(struct timespec) pureconst;
struct timespec _timespec_add(struct timespec, struct timespec) pureconst;
struct timespec _timespec_fromnanos(int64_t) pureconst;
struct timespec _timespec_frommicros(int64_t) pureconst;
struct timespec _timespec_frommillis(int64_t) pureconst;
struct timespec _timespec_real(void);
struct timespec _timespec_mono(void);
struct timespec _timespec_sleep(struct timespec);
int _timespec_sleep_until(struct timespec);
struct timespec _timespec_sub(struct timespec, struct timespec) pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
