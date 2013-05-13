/*-
 * Copyright (c) 1999-2003 MAEKAWA Masahide <gehenna@daemon-systems.org>
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
 *	$Id: main.h,v 1.15 2003/01/11 14:46:36 maekawa Exp $
 */

#ifndef	UPACKED
#define	UPACKED	__attribute__((__packed__))
#endif	/* UPACKED */

#define	BIT(x, k, n)	(((x) >> (k)) & ~(~0 << (n)))

int usb_get_descriptor(int, uint8_t, uint8_t, uint8_t, void *, size_t);
void bprintf(uint8_t *, size_t);
void iprintf(const char *, ...);

struct devinfo {
	uint8_t		class;
	uint8_t		subclass;
	uint8_t		protocol;
	uint16_t	vendor;
	uint16_t	product;
};

/* standard.c */
void dump_full(const struct usb_full_desc *);
void dump_descriptor(const void *, const char *);
void dump_device(const usb_device_descriptor_t *);
void dump_string(const usb_string_descriptor_t *, int);

/* Class Specific */
void dump_cs_device(usb_descriptor_t *, int);
void dump_cs_interface(usb_descriptor_t *);
void dump_cs_endpoint(usb_descriptor_t *);

#if defined(__NetBSD__) || defined(__OpenBSD__)
#define	_PATH_DEV_UGEN		"/dev/ugen0.00"
#endif /* defined(__NetBSD__) || defined(__OpenBSD__) */

#if defined(__FreeBSD__)
#define	_PATH_DEV_UGEN		"/dev/ugen0"

/* Class definitions */
#ifndef UICLASS_AUDIO
#define	UICLASS_AUDIO		UCLASS_AUDIO
#endif /* UICLASS_AUDIO */
#ifndef UISUBCLASS_AUDIOCONTROL
#define	UISUBCLASS_AUDIOCONTROL	USUBCLASS_AUDIOCONTROL
#endif /* UISUBCLASS_AUDIOCONTROL */
#ifndef UISUBCLASS_AUDIOSTREAM
#define	UISUBCLASS_AUDIOSTREAM	USUBCLASS_AUDIOSTREAM
#endif /* UISUBCLASS_AUDIOSTREAM */
#ifndef UISUBCLASS_MIDISTREAM
#define	UISUBCLASS_MIDISTREAM	USUBCLASS_MIDISTREAM
#endif /* UISUBCLASS_MIDISTREAM */
#ifndef UICLASS_HID
#define	UICLASS_HID		UCLASS_HID
#endif /* UICLASS_HID */

/* Descriptor type */
#ifndef UDESC_INTERFACE_POWER
#define	UDESC_INTERFACE_POWER	0x08
#endif /* UDESC_INTERFACE_POWER */
#endif /* defined(__FreeBSD__) */
