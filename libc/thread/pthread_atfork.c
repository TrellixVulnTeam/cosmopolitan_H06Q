/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/kmalloc.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static struct AtForks {
  pthread_mutex_t lock;
  struct AtFork {
    struct AtFork *p[2];
    atfork_f f[3];
  } * list;
} _atforks;

static void _pthread_onfork(int i) {
  struct AtFork *a;
  struct PosixThread *pt;
  _unassert(0 <= i && i <= 2);
  if (!i) pthread_mutex_lock(&_atforks.lock);
  for (a = _atforks.list; a; a = a->p[!i]) {
    if (a->f[i]) a->f[i]();
    _atforks.list = a;
  }
  if (i) pthread_mutex_unlock(&_atforks.lock);
  if (i == 2) {
    _pthread_zombies_purge();
    if (__tls_enabled) {
      pt = (struct PosixThread *)__get_tls()->tib_pthread;
      pt->flags |= PT_MAINTHREAD;
    }
  }
}

void _pthread_onfork_prepare(void) {
  _pthread_onfork(0);
}

void _pthread_onfork_parent(void) {
  _pthread_onfork(1);
}

void _pthread_onfork_child(void) {
  _pthread_onfork(2);
}

int _pthread_atfork(atfork_f prepare, atfork_f parent, atfork_f child) {
  int rc;
  struct AtFork *a;
  a = kmalloc(sizeof(struct AtFork));
  a->f[0] = prepare;
  a->f[1] = parent;
  a->f[2] = child;
  pthread_mutex_lock(&_atforks.lock);
  a->p[0] = 0;
  a->p[1] = _atforks.list;
  if (_atforks.list) _atforks.list->p[0] = a;
  _atforks.list = a;
  pthread_mutex_unlock(&_atforks.lock);
  rc = 0;
  return rc;
}
