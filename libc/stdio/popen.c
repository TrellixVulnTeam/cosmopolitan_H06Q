/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Spawns subprocess and returns pipe stream.
 *
 * This embeds the Cosmopolitan Command Interpreter which provides
 * Bourne-like syntax on all platforms including Windows.
 *
 * @see pclose()
 * @threadsafe
 */
FILE *popen(const char *cmdline, const char *mode) {
  FILE *f;
  int e, pid, dir, flags, pipefds[2];
  flags = fopenflags(mode);
  if ((flags & O_ACCMODE) == O_RDONLY) {
    dir = 0;
  } else if ((flags & O_ACCMODE) == O_WRONLY) {
    dir = 1;
  } else {
    einval();
    return NULL;
  }
  if (pipe2(pipefds, O_CLOEXEC) == -1) return NULL;
  if ((f = fdopen(pipefds[dir], mode))) {
    switch ((pid = fork())) {
      case 0:
        _unassert(dup2(pipefds[!dir], !dir) == !dir);
        // we can't rely on cloexec because cocmd builtins don't execve
        if (pipefds[0] != !dir) _unassert(!close(pipefds[0]));
        if (pipefds[1] != !dir) _unassert(!close(pipefds[1]));
        _Exit(_cocmd(3, (char *[]){"popen", "-c", cmdline, 0}, environ));
      default:
        f->pid = pid;
        _unassert(!close(pipefds[!dir]));
        return f;
      case -1:
        e = errno;
        _unassert(!fclose(f));
        _unassert(!close(pipefds[!dir]));
        errno = e;
        return NULL;
    }
  } else {
    e = errno;
    _unassert(!close(pipefds[0]));
    _unassert(!close(pipefds[1]));
    errno = e;
    return NULL;
  }
}
