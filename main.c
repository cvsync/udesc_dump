/*-
 * Copyright (c) 1999-2011 MAEKAWA Masahide <gehenna@daemon-systems.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: main.c,v 1.16 2011/04/26 01:29:01 maekawa Exp $
 */

#include <sys/ioctl.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <dev/usb/usb.h>

#include "main.h"

int indexes = 0, langid = 0, verbose_level = 0, fd;

static void error(const char *, ...);
static void verbose(int, const char *, ...);

extern struct devinfo usbdev;
extern int level;

int
main(int argc, char *argv[])
{
	usb_device_descriptor_t ddesc;
	struct usb_config_desc cdesc;
	struct usb_full_desc fdesc;
	struct usb_string_desc sdesc;
	const char *device_name = NULL;
	uint8_t *buffer;
	int n, i;

	if (argc > 1)
		device_name = argv[1];
#ifdef _PATH_DEV_UGEN
	else
		device_name = _PATH_DEV_UGEN;
#endif /* _PATH_DEV_UGEN */
	if (device_name == NULL) {
		error("not specified ugen device node");
		exit(EXIT_FAILURE);
	}

	if ((fd = open(device_name, O_RDWR, 0)) < 0) {
		error("%s", device_name);
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, USB_GET_DEVICE_DESC, &ddesc) != 0) {
		error("ioctl USB_GET_DEVICE_DESC");
		close(fd);
		exit(EXIT_FAILURE);
	}

	dump_device(&ddesc);

	for (i = 0 ; i < ddesc.bNumConfigurations ; i++) {
		cdesc.ucd_config_index = i;
		if (ioctl(fd, USB_GET_CONFIG_DESC, &cdesc) != 0) {
			error("ioctl USB_GET_CONFIG_DESC");
			close(fd);
			exit(EXIT_FAILURE);
		}

		fdesc.ufd_config_index = i;
		fdesc.ufd_size = UGETW(cdesc.ucd_desc.wTotalLength);
		if ((fdesc.ufd_data = malloc(fdesc.ufd_size)) == NULL) {
			error("config %d", i);
			close(fd);
			exit(EXIT_FAILURE);
		}

		if (ioctl(fd, USB_GET_FULL_DESC, &fdesc) != 0) {
			error("ioctl USB_GET_FULL_DESC");
			free(fdesc.ufd_data);
			close(fd);
			exit(EXIT_FAILURE);
		}

		verbose(-1, "Configuration %d:", i);

		dump_full(&fdesc);

		free(fdesc.ufd_data);
	}

	if (indexes != 0) {
		memset(&sdesc, 0, sizeof(sdesc));
		if (ioctl(fd, USB_GET_STRING_DESC, &sdesc) != 0) {
			error("ioctl USB_GET_STRING_DESC");
			close(fd);
			exit(EXIT_FAILURE);
		}

		dump_string(&sdesc.usd_desc, 0);

		buffer = (uint8_t *)(void *)(&sdesc.usd_desc.bString);
		n = (sdesc.usd_desc.bLength - 2) / 2;

		for (i = 0 ; i < n ; i++) {
			langid = UGETW(&buffer[i*2]);
			if (langid == 0x0409)
				break;
		}

		if (langid == 0x0409) {
			sdesc.usd_language_id = langid;

			for (i = 1 ; i <= indexes ; i++) {
				sdesc.usd_string_index = i;
				if (ioctl(fd, USB_GET_STRING_DESC,
					  &sdesc) != 0) {
					error("ioctl USB_GET_STRING_DESC");
					close(fd);
					exit(EXIT_FAILURE);
				}

				dump_string(&sdesc.usd_desc, i);
			}
		}
	}

	close(fd);

	exit(EXIT_SUCCESS);
	/* NOTREACHED */
}

int
usb_get_descriptor(int addr, uint8_t vh, uint8_t vl, uint8_t idx, void *buffer,
		   size_t bufsize)
{
	struct usb_ctl_request ctlreq;

	ctlreq.ucr_addr = addr;

	ctlreq.ucr_request.bmRequestType = UT_READ_INTERFACE;
	ctlreq.ucr_request.bRequest = UR_GET_DESCRIPTOR;
	USETW2(ctlreq.ucr_request.wValue, vh, vl);
	USETW(ctlreq.ucr_request.wIndex, idx);
	USETW(ctlreq.ucr_request.wLength, bufsize);

	ctlreq.ucr_data = buffer;
	ctlreq.ucr_flags = 0;
	ctlreq.ucr_actlen = 0;

	if (ioctl(fd, USB_DO_REQUEST, &ctlreq) != 0)
		return (0);

	if ((size_t)ctlreq.ucr_actlen != bufsize)
		return (0);

	return (1);
}

void
bprintf(uint8_t *buffer, size_t bufsize)
{
	int i, bitlen;

	bitlen = bufsize * 8;
	for (i = 0 ; i < bitlen ; i++)
		printf("%d", BIT(buffer[i / 8], 7 - (i % 8), 1));
}

void
iprintf(const char *fmt, ...)
{
	va_list ap;
	int i;

	for (i = 0 ; i < level ; i++)
		printf("\t");

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
}

static void
error(const char *fmt, ...)
{
	va_list ap;
	int sv_errno = errno;

	fflush(stdout);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, ": %s\n", strerror(sv_errno));
}

static void
verbose(int val, const char *fmt, ...)
{
	va_list ap;

	if (verbose_level < val)
		return;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	fprintf(stdout, "\n");
}
