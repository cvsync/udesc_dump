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
 *	$Id: standard.c,v 1.18 2011/04/26 01:29:01 maekawa Exp $
 */

#include <stdio.h>

#include <ctype.h>
#include <unistd.h>

#include <dev/usb/usb.h>

#include "main.h"

struct devinfo usbdev;

static void dump_config(const usb_config_descriptor_t *);
static int dump_interface(const usb_descriptor_t *);
static void dump_interface_power(const usb_descriptor_t *);
static void dump_endpoint(const usb_descriptor_t *);

extern int indexes;

void
dump_full(const struct usb_full_desc *fdesc)
{
	usb_config_descriptor_t *cdesc;
	usb_descriptor_t *desc = (usb_descriptor_t *)fdesc->ufd_data;
	uint8_t *buffer = (uint8_t *)fdesc->ufd_data;
	int wTotalLength = 0, current_interface = 0, i;

	if (desc->bDescriptorType != UDESC_CONFIG) {
		puts("must be started from configuration descriptor.");
		return;
	}

	cdesc = (usb_config_descriptor_t *)desc;
	wTotalLength = UGETW(cdesc->wTotalLength);

	for (i = 0 ; i < wTotalLength ; i += desc->bLength) {
		desc = (usb_descriptor_t *)&buffer[i];

		switch (desc->bDescriptorType) {
		case UDESC_CONFIG:
			dump_config((usb_config_descriptor_t *)desc);
			break;
		case UDESC_INTERFACE:
			current_interface = dump_interface(desc);
			break;
		case UDESC_ENDPOINT:
			dump_endpoint(desc);
			break;
		case UDESC_INTERFACE_POWER:
			dump_interface_power(desc);
			break;
		case UDESC_CS_DEVICE:
			dump_cs_device(desc, current_interface);
			break;
		case UDESC_CS_INTERFACE:
			dump_cs_interface(desc);
			break;
		case UDESC_CS_ENDPOINT:
			dump_cs_endpoint(desc);
			break;
		default:
			dump_descriptor(&buffer[i], "\t\t");
			break;
		}
	}
}

void
dump_descriptor(const void *p, const char *indent)
{
	const usb_descriptor_t *desc = (const usb_descriptor_t *)p;
	const uint8_t *buffer = (const uint8_t *)p;
	int i;

	printf("%sDescriptor:\n%s  bLength            %d\n"
	       "%s  bDescriptorType    %02x\n",
	       indent, indent, desc->bLength, indent, desc->bDescriptorType);

	printf("%s", indent);

	for (i = 0 ; i < buffer[0] ; i++)
		printf("%02x ", buffer[i]);

	printf("\n\n");
}

void
dump_device(const usb_device_descriptor_t *ddesc)
{
	usbdev.vendor  = UGETW(ddesc->idVendor);
	usbdev.product = UGETW(ddesc->idProduct);

	printf("Standard Device Descriptor:\n  bLength            %d\n"
	       "  bDescriptorType    %02x\n  bcdUSB             %04x\n"
	       "  bDeviceClass       %02x\n  bDeviceSubClass    %02x\n"
	       "  bDeviceProtocol    %02x\n  bMaxPacketSize     %d\n"
	       "  idVendor           %04x\n  idProduct          %04x\n"
	       "  bcdDevice          %04x\n  iManufacturer      %d\n"
	       "  iProduct           %d\n  iSerialNumber      %d\n"
	       "  bNumConfigurations %d\n\n",
	       ddesc->bLength, ddesc->bDescriptorType, UGETW(ddesc->bcdUSB),
	       ddesc->bDeviceClass, ddesc->bDeviceSubClass,
	       ddesc->bDeviceProtocol, ddesc->bMaxPacketSize,
	       UGETW(ddesc->idVendor), UGETW(ddesc->idProduct),
	       UGETW(ddesc->bcdDevice), ddesc->iManufacturer, ddesc->iProduct,
	       ddesc->iSerialNumber, ddesc->bNumConfigurations);

	if (ddesc->iManufacturer > indexes)
		indexes = ddesc->iManufacturer;
	if (ddesc->iProduct > indexes)
		indexes = ddesc->iProduct;
	if (ddesc->iSerialNumber > indexes)
		indexes = ddesc->iSerialNumber;
}

static void
dump_config(const usb_config_descriptor_t *cdesc)
{
	int powered = 0, wakeup = 0;

	if (cdesc->bmAttributes & UC_SELF_POWERED)
		powered = 1;
	if (cdesc->bmAttributes & UC_REMOTE_WAKEUP)
		wakeup = 1;

	printf("\tStandard Configuration Descriptor:\n"
	       "\t  bLength             %d\n\t  bDescriptorType     %02x\n"
	       "\t  wTotalLength        %d\n\t  bNumInterface       %d\n"
	       "\t  bConfigurationValue %d\n\t  iConfiguration      %d\n"
	       "\t  bmAttributes        %02x%s%s%s%s%s\n"
	       "\t  bMaxPower           %d (%d mA)\n\n",
	       cdesc->bLength, cdesc->bDescriptorType,
	       UGETW(cdesc->wTotalLength), cdesc->bNumInterface,
	       cdesc->bConfigurationValue, cdesc->iConfiguration,
	       cdesc->bmAttributes, (powered || wakeup) ? " (" : "",
	       powered ? "self-powered" : "", (powered && wakeup) ? ", " : "",
	       wakeup ? "remote-wakeup" : "", (powered || wakeup) ? ")" : "",
	       cdesc->bMaxPower, cdesc->bMaxPower * 2);

	if (cdesc->iConfiguration > indexes)
		indexes = cdesc->iConfiguration;
}

static int
dump_interface(const usb_descriptor_t *desc)
{
	const usb_interface_descriptor_t *idesc =
		(const usb_interface_descriptor_t *)desc;

	usbdev.class    = idesc->bInterfaceClass;
	usbdev.subclass = idesc->bInterfaceSubClass;
	usbdev.protocol = idesc->bInterfaceProtocol;

	printf("\tStandard Interface Descriptor:\n\t  bLength            %d\n"
	       "\t  bDescriptorType    %02x\n\t  bInterfaceNumber   %d\n"
	       "\t  bAlternateSetting  %d\n\t  bNumEndpoints      %d\n"
	       "\t  bInterfaceClass    %02x\n\t  bInterfaceSubClass %02x\n"
	       "\t  bInterfaceProtocol %02x\n\t  iInterface         %d\n\n",
	       idesc->bLength, idesc->bDescriptorType, idesc->bInterfaceNumber,
	       idesc->bAlternateSetting, idesc->bNumEndpoints,
	       idesc->bInterfaceClass, idesc->bInterfaceSubClass,
	       idesc->bInterfaceProtocol, idesc->iInterface);

	if (idesc->iInterface > indexes)
		indexes = idesc->iInterface;

	return (idesc->bInterfaceNumber);
}

static void
dump_interface_power(const usb_descriptor_t *desc)
{
	const uint8_t *buffer = (const uint8_t *)desc;
	uint8_t bitmap;

	printf("\tStandard Interface Power Descriptor:\n"
	       "\t  bLength             %d\n\t  bDescriptorType     %02x\n"
	       "\t  bmCapabilitiesFlags %02x",
	       buffer[0], buffer[1], buffer[2]);

	bitmap = buffer[2];

	if (bitmap & 0x0f) {
		printf(" (receives ");
		if (bitmap & 0x01)
			printf("D0");
		if (bitmap & 0x02) {
			if (bitmap & 0x01)
				printf(", ");
			printf("D1");
		}
		if (bitmap & 0x04) {
			if (bitmap & 0x03)
				printf(", ");
			printf("D2");
		}
		if (bitmap & 0x08) {
			if (bitmap & 0x07)
				printf(", ");
			printf("D3");
		}
	}
	if (bitmap & 0x30) {
		if (bitmap & 0x0f)
			printf(", ");
		else
			printf("(");
		printf("wakeup from ");
		if (bitmap & 0x10)
			printf("D1");
		if (bitmap & 0x20) {
			if (bitmap & 0x10)
				printf(", ");
			printf("D2");
		}
	}
	if (bitmap != 0)
		printf(")");

	printf("\n\n");
}

static void
dump_endpoint(const usb_descriptor_t *desc)
{
	const usb_endpoint_descriptor_t *edesc =
		(const usb_endpoint_descriptor_t *)desc;
	const char *xfer_type;

	printf("\tStandard Endpoint Descriptor:\n");

	switch (edesc->bmAttributes & UE_XFERTYPE) {
	case UE_CONTROL:
		xfer_type = "Control";
		break;
	case UE_ISOCHRONOUS:
		switch (edesc->bmAttributes & UE_ISO_TYPE) {
		case UE_ISO_ASYNC:
			xfer_type = "Isochronous-Asynchronous";
			break;
		case UE_ISO_ADAPT:
			xfer_type = "Isochronous-Adaptive";
			break;
		case UE_ISO_SYNC:
			xfer_type = "Isochronous-Synchronous";
			break;
		default:
			xfer_type = "Isochronous";
		}
		break;
	case UE_BULK:
		xfer_type = "Bulk";
		break;
	case UE_INTERRUPT:
		xfer_type = "Interruput";
		break;
	default:
		xfer_type = "unknown";
	}

	printf("\t  bLength          %d\n\t  bDescriptorType  %02x\n"
	       "\t  bEndpointAddress %02x (%s)\n"
	       "\t  bmAttributes     %02x (%s)\n\t  wMaxPacketSize   %d\n"
	       "\t  bInterval        %d\n",
	       edesc->bLength, edesc->bDescriptorType, edesc->bEndpointAddress,
	       UE_GET_DIR(edesc->bEndpointAddress) == UE_DIR_IN ? "in" : "out",
	       edesc->bmAttributes, xfer_type, UGETW(edesc->wMaxPacketSize),
	       edesc->bInterval);

	if (usbdev.class == UICLASS_AUDIO) {
		const uint8_t *buffer = (const uint8_t *)edesc;
		printf("\t  bRefresh         %d\n\t  bSynchAddress    %02x\n",
		       buffer[7], buffer[8]);
	}

	printf("\n");
}

void
dump_string(const usb_string_descriptor_t *sdesc, int idx)
{
	uint8_t *buffer = (uint8_t *)sdesc->bString;
	int n, i;

	n = (sdesc->bLength - 2) / 2;

	if (idx == 0) {
		printf("Codes Representing Languages by the Device:\n");

		printf("  bLength          %d\n  bDescriptorType  %02x\n",
		       sdesc->bLength, sdesc->bDescriptorType);

		for (i = 0 ; i < n ; i++) {
			printf("  wLANGID[%d]       %04x\n",
			       i, UGETW(&buffer[i*2]));
		}
	} else {
		printf("String (index %d): ", idx);
		for (i = 0 ; i < n ; i++)
			printf("%c", isprint(UGETW(&buffer[i*2])) ?
			       UGETW(&buffer[i*2]) : '.');
		printf("\n");
	}

	printf("\n");
}
