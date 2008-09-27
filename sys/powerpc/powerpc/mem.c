/*-
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1982, 1986, 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department, and code derived from software contributed to
 * Berkeley by William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: Utah $Hdr: mem.c 1.13 89/10/08$
 *	from: @(#)mem.c	7.2 (Berkeley) 5/9/91
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*
 * Memory special file
 */

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/fcntl.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/memrange.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/msgbuf.h>
#include <sys/systm.h>
#include <sys/signalvar.h>
#include <sys/uio.h>

#include <machine/md_var.h>
#include <machine/vmparam.h>

#include <vm/vm.h>
#include <vm/pmap.h>
#include <vm/vm_extern.h>

#include <machine/memdev.h>

struct mem_range_softc mem_range_softc;

/* ARGSUSED */
int
memrw(struct cdev *dev, struct uio *uio, int flags)
{
	struct iovec *iov;
	int error = 0;
	vm_offset_t va, eva, off, v;
	vm_prot_t prot;
	vm_size_t cnt;

	cnt = 0;
	error = 0;

	GIANT_REQUIRED;

	while (uio->uio_resid > 0 && !error) {
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			if (uio->uio_iovcnt < 0)
				panic("memrw");
			continue;
		}
		if (dev2unit(dev) == CDEV_MINOR_MEM) {
kmem_direct_mapped:	v = uio->uio_offset;

			off = uio->uio_offset & PAGE_MASK;
			cnt = PAGE_SIZE - ((vm_offset_t)iov->iov_base &
			    PAGE_MASK);
			cnt = min(cnt, PAGE_SIZE - off);
			cnt = min(cnt, iov->iov_len);

			if (mem_valid(v, cnt)
			    && pmap_dev_direct_mapped(v, cnt)) {
				error = EFAULT;
				break;
			}

			uiomove((void *)v, cnt, uio);
			break;
		}
		else if (dev2unit(dev) == CDEV_MINOR_KMEM) {
			va = uio->uio_offset;

			if ((va < VM_MIN_KERNEL_ADDRESS)
			    || (va > VM_MAX_KERNEL_ADDRESS))
				goto kmem_direct_mapped;

			va = trunc_page(uio->uio_offset);
			eva = round_page(uio->uio_offset
			    + iov->iov_len);

			/* 
			 * Make sure that all the pages are currently resident
			 * so that we don't create any zero-fill pages.
			 */

			for (; va < eva; va += PAGE_SIZE)
				if (pmap_extract(kernel_pmap, va)
				    == 0)
					return (EFAULT);

			prot = (uio->uio_rw == UIO_READ)
			    ? VM_PROT_READ : VM_PROT_WRITE;

			va = uio->uio_offset;
			if (kernacc((void *) va, iov->iov_len, prot)
			    == FALSE)
				return (EFAULT);

			error = uiomove((void *)va, iov->iov_len, uio);

			continue;
		}
	}

	return (error);
}

/*
 * allow user processes to MMAP some memory sections
 * instead of going through read/write
 */
int
memmmap(struct cdev *dev, vm_offset_t offset, vm_paddr_t *paddr, int prot)
{
	/*
	 * /dev/mem is the only one that makes sense through this
	 * interface.  For /dev/kmem any physaddr we return here
	 * could be transient and hence incorrect or invalid at
	 * a later time.
	 */
	if (dev2unit(dev) != CDEV_MINOR_MEM)
		return (-1);

	/* Only direct-mapped addresses. */
	if (mem_valid(offset, 0)
	    && pmap_dev_direct_mapped(offset, 0))
		return (EFAULT);

	*paddr = offset;

	return (0);
}

void
dev_mem_md_init(void)
{
}
