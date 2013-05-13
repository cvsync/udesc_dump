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
 *	$Id: cs.c,v 1.10 2003/07/30 05:37:28 maekawa Exp $
 */

/*
 * Class Specific
 */

#include <dev/usb/usb.h>

#include "main.h"
#include "audio.h"
#include "hid.h"

extern struct devinfo usbdev;

void
dump_cs_device(usb_descriptor_t *desc, int interface)
{
	switch (usbdev.class) {
	case UICLASS_HID:
		hid_device(desc, interface);
		break;
	default:
		dump_descriptor(desc, "\t\t");
	}
}

void
dump_cs_interface(usb_descriptor_t *desc)
{
	if (usbdev.vendor == 0x0499 && usbdev.product == 0x1000) {
		/* YAMAHA UX256 USB MIDI */
		audio_midi_interface(desc);
		return;
	}
	if (usbdev.vendor == 0x0582 && usbdev.product == 0x0050) {
		/* Roland EDIROL UA-3FX USB audio I/F (advanced) */
		audio_interface(desc, usbdev.subclass);
		return;
	}

	switch (usbdev.class) {
	case UICLASS_AUDIO:
		audio_interface(desc, usbdev.subclass);
		break;
	default:
		dump_descriptor(desc, "\t\t");
	}
}

void
dump_cs_endpoint(usb_descriptor_t *desc)
{
	if (usbdev.vendor == 0x0582 && usbdev.product == 0x0050) {
		/* Roland EDIROL UA-3FX USB audio I/F (advanced) */
		audio_endpoint(desc, usbdev.subclass);
		return;
	}

	switch (usbdev.class) {
	case UICLASS_AUDIO:
		audio_endpoint(desc, usbdev.subclass);
		break;
	default:
		dump_descriptor(desc, "\t\t");
	}
}
