/*
 * Copyright (c) 2000-2007 Niels Provos <provos@citi.umich.edu>
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef EVENT1_EVENT_H_INCLUDED_
#define EVENT1_EVENT_H_INCLUDED_

/** @file event.h

  A library for writing event-driven network servers.

  The <event.h> header is deprecated in Libevent 2.0 and later; please
  use
  <event.h>
  instead.  Depending on what functionality you need, you may also want to
  include more of the other event2/ headers.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <event-config.h>
#ifdef EVENT__HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef EVENT__HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef EVENT__HAVE_STDINT_H
#include <stdint.h>
#endif
#include <stdarg.h>

/* For int types. */
#include <evutil.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <buffer.h>
#include <buffer_compat.h>
#include <bufferevent.h>
#include <bufferevent_compat.h>
#include <bufferevent_struct.h>
#include <event.h>
#include <event_compat.h>
#include <event_struct.h>
#include <tag.h>
#include <tag_compat.h>

#ifdef __cplusplus
}
#endif

#endif /* EVENT1_EVENT_H_INCLUDED_ */
