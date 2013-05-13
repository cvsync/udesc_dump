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
 *	$Id: audio.c,v 1.19 2011/04/26 01:29:01 maekawa Exp $
 */

#include <stdio.h>

#include <unistd.h>

#include <dev/usb/usb.h>

#include "main.h"
#include "audio.h"
#include "uaudioreg.h"

static void audio_control_header(const usb_descriptor_t *);
static void audio_control_input_terminal(const usb_descriptor_t *);
static void audio_control_output_terminal(const usb_descriptor_t *);
static void audio_control_mixer_unit(const usb_descriptor_t *);
static void audio_control_selector_unit(const usb_descriptor_t *);
static void audio_control_feature_unit(const usb_descriptor_t *);
static void audio_control_processing_unit(const usb_descriptor_t *);
static void audio_control_extension_unit(const usb_descriptor_t *);
static void audio_streaming_general(const uint8_t *);
static void audio_streaming_format(const uint8_t *);
static void audio_streaming_format_type1(const uint8_t *);
static void audio_streaming_format_type2(const uint8_t *);
static void audio_streaming_format_type3(const uint8_t *);
static void audio_streaming_format_specific(const uint8_t *);
static void audio_streaming_format_mpeg(const uint8_t *);
static void audio_streaming_format_ac3(const uint8_t *);
static void audio_midi_header(const uint8_t *);
static void audio_midi_midi_in_jack(const uint8_t *);
static void audio_midi_midi_out_jack(const uint8_t *);
static void audio_midi_element(const uint8_t *);

/*
 * Class: Audio
 */
void
audio_interface(usb_descriptor_t *desc, uint16_t subclass)
{
	switch (subclass) {
	case UISUBCLASS_AUDIOCONTROL:
		audio_control_interface(desc);
		break;
	case UISUBCLASS_AUDIOSTREAM:
		audio_streaming_interface(desc);
		break;
	case UISUBCLASS_MIDISTREAM:
		audio_midi_interface(desc);
		break;
	default:
		dump_descriptor(desc, "\t\t");
		break;
	}
}

void
audio_endpoint(usb_descriptor_t *desc, uint16_t subclass)
{
	switch (subclass) {
	case UISUBCLASS_AUDIOSTREAM:
		audio_streaming_endpoint(desc);
		break;
	case UISUBCLASS_MIDISTREAM:
		audio_midi_endpoint(desc);
		break;
	default:
		dump_descriptor(desc, "\t\t");
		break;
	}
}

/*
 * Subclass: Audio Control
 */
void
audio_control_interface(usb_descriptor_t *desc)
{
	const struct usb_audio_control_descriptor *adesc =
		(const struct usb_audio_control_descriptor *)desc;

	switch (adesc->bDescriptorSubtype) {
	case UDESCSUB_AC_HEADER:
		audio_control_header(desc);
		break;
	case UDESCSUB_AC_INPUT:
		audio_control_input_terminal(desc);
		break;
	case UDESCSUB_AC_OUTPUT:
		audio_control_output_terminal(desc);
		break;
	case UDESCSUB_AC_MIXER:
		audio_control_mixer_unit(desc);
		break;
	case UDESCSUB_AC_SELECTOR:
		audio_control_selector_unit(desc);
		break;
	case UDESCSUB_AC_FEATURE:
		audio_control_feature_unit(desc);
		break;
	case UDESCSUB_AC_PROCESSING:
		audio_control_processing_unit(desc);
		break;
	case UDESCSUB_AC_EXTENSION:
		audio_control_extension_unit(desc);
		break;
	default:
		dump_descriptor(desc, "\t\t");
	}
}

static void
audio_control_header(const usb_descriptor_t *desc)
{
	const struct usb_audio_control_descriptor *adesc =
		(const struct usb_audio_control_descriptor *)desc;
	uint8_t i;

	printf("\t\tClass-Specific AC Interface Header Descriptor:\n"
	       "\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bcdADC             %04x\n"
	       "\t\t  wTotalLength       %u\n"
	       "\t\t  bInCollection      %u\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, UGETW(adesc->bcdADC),
	       UGETW(adesc->wTotalLength), adesc->bInCollection);

	for (i = 0 ; i < adesc->bInCollection ; i++) {
		printf("\t\t  baInterfaceNr(%u)   %u\n", i+1,
		       adesc->baInterfaceNr[i]);
	}

	printf("\n");
}

static void
audio_control_input_terminal(const usb_descriptor_t *desc)
{
	const struct usb_audio_input_terminal *adesc =
		(const struct usb_audio_input_terminal *)desc;

	printf("\t\tInput Terminal Descriptor:\n\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bTerminalID        %u\n\t\t  wTerminalType      %04x\n"
	       "\t\t  bAssocTerminal     %u\n\t\t  bNrChannels        %u\n"
	       "\t\t  wChannelConfig     %04x\n\t\t  iChannelNames      %u\n"
	       "\t\t  iTerminal          %u\n\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bTerminalId,
	       UGETW(adesc->wTerminalType), adesc->bAssocTerminal,
	       adesc->bNrChannels, UGETW(adesc->wChannelConfig),
	       adesc->iChannelNames, adesc->iTerminal);
}

static void
audio_control_output_terminal(const usb_descriptor_t *desc)
{
	const struct usb_audio_output_terminal *adesc =
		(const struct usb_audio_output_terminal *)desc;

	printf("\t\tOutput Terminal Descriptor:\n\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bTerminalID        %u\n\t\t  wTerminalType      %04x\n"
	       "\t\t  bAssocTerminal     %u\n\t\t  bSourceID          %u\n"
	       "\t\t  iTerminal          %u\n\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bTerminalId,
	       UGETW(adesc->wTerminalType), adesc->bAssocTerminal,
	       adesc->bSourceId, adesc->iTerminal);
}

static void
audio_control_mixer_unit(const usb_descriptor_t *desc)
{
	const struct usb_audio_mixer_unit *adesc =
		(const struct usb_audio_mixer_unit *)desc;
	const struct usb_audio_mixer_unit_1 *adesc1;
	uint8_t i;

	printf("\t\tMixer Unit Descriptor:\n\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bUnitID            %u\n\t\t  bNrInPins          %u\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bUnitId, adesc->bNrInPins);

	for (i = 0 ; i < adesc->bNrInPins ; i++) {
		printf("\t\t  baSourceID(%u)      %u\n", i+1,
		       adesc->baSourceId[i]);
	}

	adesc1 = (const struct usb_audio_mixer_unit_1 *)
			&adesc->baSourceId[adesc->bNrInPins];

	printf("\t\t  bNrChannels        %u\n\t\t  wChannelConfig     %u\n"
	       "\t\t  iChannelNames      %u\n",
	       adesc1->bNrChannels, UGETW(adesc1->wChannelConfig),
	       adesc1->iChannelNames);

	for (i = 0 ; i < adesc1->bNrChannels ; i++) {
		printf("\t\t  bmControls(%u)      %u\n", i,
		       adesc1->bmControls[i]);
	}

	printf("\t\t  iMixer             %u\n\n",
	       adesc1->bmControls[adesc1->bNrChannels]);
}

static void
audio_control_selector_unit(const usb_descriptor_t *desc)
{
	const struct usb_audio_selector_unit *adesc =
		(const struct usb_audio_selector_unit *)desc;
	uint8_t i;

	printf("\t\tSelector Unit Descriptor:\n\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bUnitID            %u\n\t\t  bNrInPins          %u\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bUnitId, adesc->bNrInPins);

	for (i = 0 ; i < adesc->bNrInPins ; i++) {
		printf("\t\t  baSourceID(%u)     %u\n", i+1,
		       adesc->baSourceId[i]);
	}

	printf("\t\t  iSelector          %u\n",
	       adesc->baSourceId[adesc->bNrInPins]);

	printf("\n");
}

static void
audio_control_feature_unit(const usb_descriptor_t *desc)
{
	const struct usb_audio_feature_unit *adesc =
		(const struct usb_audio_feature_unit *)desc;
	uint8_t ch, i, j;

	printf("\t\tFeature Unit Descriptor:\n\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bUnitID            %u\n\t\t  bSourceID          %u\n"
	       "\t\t  bControlSize       %u\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bUnitId,
	       adesc->bSourceId, adesc->bControlSize);

	ch = (adesc->bLength - 7) / adesc->bControlSize;

	for (i = 0 ; i <= ch ; i++) {
		for (j = 0 ; j < adesc->bControlSize ; j++) {
			printf("\t\t  bmaControls(%u)     %02x\n",
			       i, adesc->bmaControls[i*j]);
		}
	}

	printf("\t\t  iFeature           %u\n\n",
	       adesc->bmaControls[ch*adesc->bControlSize]);
}

static void
audio_control_processing_unit(const usb_descriptor_t *desc)
{
	const struct usb_audio_processing_unit *adesc =
		(const struct usb_audio_processing_unit *)desc;
	const struct usb_audio_processing_unit_1 *adesc1;
	/* XXX - updown should be 2? */
	const struct usb_audio_processing_unit_updown *adesc2;
	uint8_t i;
	int mode = 0;

	switch (UGETW(adesc->wProcessType)) {
	case UPDOWNMIX_PROCESS:
		printf("\t\tUp/Down-mix Processing Unit Descriptor:\n");
		mode = 1;
		break;
	case DOLBY_PROLOGIC_PROCESS:
		printf("\t\tDolby Prologic Processing Unit Descriptor:\n");
		mode = 1;
		break;
	case P3D_STEREO_EXTENDER_PROCESS:
		printf("\t\t3D-Stereo Extender Processing Unit Descriptor:\n");
		break;
	case REVERBATION_PROCESS:
		printf("\t\tReverberation Processing Unit Descriptor:\n");
		break;
	case CHORUS_PROCESS:
		printf("\t\tChorus Processing Unit Descriptor:\n");
		break;
	case DYN_RANGE_COMP_PROCESS:
		printf("\t\tDynamic Range Compressor"
		       " Processing Unit Descriptor:\n");
		break;
	default:
		dump_descriptor(desc, "\t\t");
		return;
	}

	printf("\t\t  bLength            %u\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  bUnitID            %u\n"
	       "\t\t  wProcessType       %04x\n\t\t  bNrInPins          %u\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bUnitId,
	       UGETW(adesc->wProcessType), adesc->bNrInPins);

	if (adesc->bNrInPins > 1) {
		for (i = 0 ; i < adesc->bNrInPins ; i++) {
			printf("\t\t  baSourceID(%u)      %u\n", i+1,
			      adesc->baSourceId[i]);
		}
	} else {
		printf("\t\t  bSourceID          %u\n", adesc->baSourceId[0]);
	}

	adesc1 = (const struct usb_audio_processing_unit_1 *)
		&adesc->baSourceId[adesc->bNrInPins];

	printf("\t\t  bNrChannels        %u\n\t\t  wChannelConfig     %04x\n"
	       "\t\t  iChannelNames      %u\n""\t\t  bControlSize       %u\n",
	       adesc1->bNrChannels, UGETW(adesc1->wChannelConfig),
	       adesc1->iChannelNames, adesc1->bControlSize);

	for (i = 0 ; i < adesc1->bControlSize ; i++) {
		printf("\t\t  bmControls(%u)      %02x\n", i+1,
		       adesc1->bmControls[i]);
	}

	adesc2 = (const struct usb_audio_processing_unit_updown *)
			&adesc1->bmControls[adesc1->bControlSize];

	printf("\t\t  iProcessing        %u\n", adesc2->iProcessing);

	if (mode) {
		printf("\t\t  bNrModes           %u\n", adesc2->bNrModes);

		for (i = 0 ; i < adesc2->bNrModes ; i++) {
			printf("\t\t  waModes(%u)         %02x\n",
			       i+1, UGETW(adesc2->waModes[i]));
		}
	}

	printf("\n");
}

static void
audio_control_extension_unit(const usb_descriptor_t *desc)
{
	const struct usb_audio_extension_unit *adesc =
		(const struct usb_audio_extension_unit *)desc;
	const struct usb_audio_extension_unit_1 *adesc1;
	uint8_t n, i;

	printf("\t\tExtension Unit Descriptor:\n\t\t  bLength            %u\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bUnitID            %u\n\t\t  wExtensionCode     %04x\n"
	       "\t\t  bNrInPins          %u\n",
	       adesc->bLength, adesc->bDescriptorType,
	       adesc->bDescriptorSubtype, adesc->bUnitId,
	       UGETW(adesc->wExtensionCode), adesc->bNrInPins);

	for (i = 0 ; i < adesc->bNrInPins ; i++) {
		printf("\t\t  baSourceID(%u)      %u\n", i+1,
		       adesc->baSourceId[i]);
	}

	adesc1 = (const struct usb_audio_extension_unit_1 *)
			&adesc->baSourceId[adesc->bNrInPins];

	printf("\t\t  bNrChannels        %u\n\t\t  wChannelConfig     %04x\n"
	       "\t\t  iChannelNames      %u\n\t\t  bControlSize       %u\n",
	       adesc1->bNrChannels, UGETW(adesc1->wChannelConfig),
	       adesc1->iChannelNames, adesc1->bControlSize);

	n = adesc->bLength - 13 - adesc->bNrInPins;

	for (i = 0 ; i < n ; i++) {
		printf("\t\t  bmControls         %02x\n",
		       adesc1->bmControls[i]);
	}

	printf("\t\t  iExtension         %u\n\n", adesc1->bmControls[n]);
}

/*
 * Subclass: Audio Streaming
 */

void
audio_streaming_interface(usb_descriptor_t *desc)
{
	const struct usb_audio_control_descriptor *adesc =
		(const struct usb_audio_control_descriptor *)desc;
	uint8_t *buffer = (uint8_t *)desc;

	switch (adesc->bDescriptorSubtype) {
	case AS_GENERAL:
		audio_streaming_general(buffer);
		break;
	case FORMAT_TYPE:
		audio_streaming_format(buffer);
		break;
	case FORMAT_SPECIFIC:
		audio_streaming_format_specific(buffer);
		break;
	default:
		dump_descriptor(buffer, "\t\t");
	}
}

void
audio_streaming_endpoint(usb_descriptor_t *desc)
{
	const struct usb_audio_streaming_endpoint_descriptor *edesc =
		(const struct usb_audio_streaming_endpoint_descriptor *)desc;

	printf("\t\tClass-Specific AS Endpoint Descriptor:\n"
	       "\t\t  bLength            %d\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  bmAttributes       %02x\n"
	       "\t\t  bLockDelayUnits    %d\n\t\t  wLockDelay         %d\n\n",
	       edesc->bLength, edesc->bDescriptorType,
	       edesc->bDescriptorSubtype, edesc->bmAttributes,
	       edesc->bLockDelayUnits, UGETW(edesc->wLockDelay));
}

static void
audio_streaming_general(const uint8_t *buffer)
{
	const struct usb_audio_streaming_interface_descriptor *idesc =
		(const struct usb_audio_streaming_interface_descriptor *)buffer;

	printf("\t\tClass-Specific AS Interface Descriptor:\n"
	       "\t\t  bLength            %d\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  bTerminalLink      %d\n"
	       "\t\t  bDecay             %d\n\t\t  wFormatTag         %d\n\n",
	       idesc->bLength, idesc->bDescriptorType,
	       idesc->bDescriptorSubtype, idesc->bTerminalLink, idesc->bDelay,
	       UGETW(idesc->wFormatTag));
}

static void
audio_streaming_format(const uint8_t *buffer)
{
	switch (buffer[3]) {
	case FORMAT_TYPE_I:
		audio_streaming_format_type1(buffer);
		break;
	case FORMAT_TYPE_II:
		audio_streaming_format_type2(buffer);
		break;
	case FORMAT_TYPE_III:
		audio_streaming_format_type3(buffer);
		break;
	default:
		dump_descriptor(buffer, "\t\t");
	}
}

static void
audio_streaming_format_type1(const uint8_t *buffer)
{
	const struct usb_audio_streaming_type1_descriptor *desc =
		(const struct usb_audio_streaming_type1_descriptor *)buffer;

	printf("\t\tType I Format Descriptor:\n\t\t  bLength            %d\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bFormatType        %02x\n\t\t  bNrChannels        %d\n"
	       "\t\t  bSubframeSize      %d\n\t\t  bBitResolution     %d\n"
	       "\t\t  bSamFreqType       %d\n",
	       desc->bLength, desc->bDescriptorType, desc->bDescriptorSubtype,
	       desc->bFormatType, desc->bNrChannels, desc->bSubFrameSize,
	       desc->bBitResolution, desc->bSamFreqType);

	if (desc->bSamFreqType == UA_SAMP_CONTNUOUS) {
		printf("\t\t  tLowerSamFreq      %d\n"
		       "\t\t  tUpperSamFreq      %d\n",
		       UA_SAMP_LO(desc), UA_SAMP_HI(desc));
	} else {
		uint8_t i;

		for (i = 0 ; i < desc->bSamFreqType ; i++) {
			printf("\t\t  tSamFreq[%d]        %d\n", i+1,
			       UA_GETSAMP(desc, i));
		}
	}

	printf("\n");
}

static void
audio_streaming_format_type2(const uint8_t *buffer)
{
	uint8_t bSamFreqType, i;

	printf("\t\tType II Format Descriptor:\n\t\t  bLength            %d\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bFormatType        %02x\n\t\t  wMaxBitRate        %d\n"
	       "\t\t  wSamplesPerFrame   %d\n\t\t  bSamFreqType       %d\n",
	       buffer[0], buffer[1], buffer[2], buffer[3],
	       UGETW(&buffer[4]), UGETW(&buffer[6]), buffer[8]);

	bSamFreqType = buffer[8];

	if (bSamFreqType == UA_SAMP_CONTNUOUS) {
		printf("\t\t  tLowerSamFreq      %d\n"
		       "\t\t  tUpperSamFreq      %d\n",
		       buffer[9] | buffer[10] << 8 | buffer[11] << 16,
		       buffer[12] | buffer[13] << 8 | buffer[14] << 16);
	} else {
		for (i = 0 ; i < bSamFreqType ; i++) {
			printf("\t\t  tSamFreq[%d]        %d\n", i+1,
			       buffer[9+i*3] | buffer[10+i*3] << 8
			       | buffer[11+i*3] << 16);
		}
	}

	printf("\n");
}

static void
audio_streaming_format_type3(const uint8_t *buffer)
{
	uint8_t bSamFreqType, i;

	printf("\t\tType III Format Descriptor:\n\t\t  bLength            %d\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bFormatType        %02x\n\t\t  bNrChannels        %d\n"
	       "\t\t  bSubframeSize      %d\n\t\t  bBitResolution     %d\n"
	       "\t\t  bSamFreqType       %d\n",
	       buffer[0], buffer[1], buffer[2], buffer[3], buffer[4],
	       buffer[5], buffer[6], buffer[7]);

	bSamFreqType = buffer[7];

	if (bSamFreqType == UA_SAMP_CONTNUOUS) {
		printf("\t\t  tLowerSamFreq      %d\n"
		       "\t\t  tUpperSamFreq      %d\n",
		       buffer[8] | buffer[9] << 8 | buffer[10] << 16,
		       buffer[11] | buffer[12] << 8 | buffer[13] << 16);
	} else {
		for (i = 0 ; i < bSamFreqType ; i++) {
			printf("\t\t  tSamFreq[%d]        %d\n", i+1,
			       buffer[8+i*3] | buffer[9+i*3] << 8
			       | buffer[10+i*3] << 16);
		}
	}

	printf("\n");
}

static void
audio_streaming_format_specific(const uint8_t *buffer)
{
	int wFormatTag = UGETW(&buffer[3]);

	switch (wFormatTag) {
	case UA_FMT_MPEG:
		audio_streaming_format_mpeg(buffer);
		break;
	case UA_FMT_AC3:
		audio_streaming_format_ac3(buffer);
		break;
	default:
		dump_descriptor(buffer, "\t\t");
	}
}

static void
audio_streaming_format_mpeg(const uint8_t *buffer)
{
	printf("\t\tMPEG Format Specific Descriptor:\n"
	       "\t\t  bLength            %d\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  wFormatTag         %04x\n"
	       "\t\t  bmMPEGCapabilities %04x\n"
	       "\t\t  bmMPEGFeatures     %02x\n\n",
	       buffer[0], buffer[1], buffer[2], UGETW(&buffer[3]),
	       UGETW(&buffer[5]), buffer[7]);
}

static void
audio_streaming_format_ac3(const uint8_t *buffer)
{
	printf("\t\tAC-3 Format Specific Descriptor:\n"
	       "\t\t  bLength            %d\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  wFormatTag         %04x\n"
	       "\t\t  bmBSID             %08x\n"
	       "\t\t  bmAC3Features      %02x\n\n",
	       buffer[0], buffer[1], buffer[2], UGETW(&buffer[3]),
	       UGETDW(&buffer[5]), buffer[9]);
}

/*
 * Subclass: MIDI Streaming
 */

void
audio_midi_interface(usb_descriptor_t *desc)
{
	const struct usb_audio_control_descriptor *adesc =
		(const struct usb_audio_control_descriptor *)desc;
	const uint8_t *buffer = (const uint8_t *)desc;

	switch (adesc->bDescriptorSubtype) {
	case USB_MIDI_MS_HEADER:
		audio_midi_header(buffer);
		break;
	case USB_MIDI_IN_JACK:
		audio_midi_midi_in_jack(buffer);
		break;
	case USB_MIDI_OUT_JACK:
		audio_midi_midi_out_jack(buffer);
		break;
	case USB_MIDI_ELEMENT:
		audio_midi_element(buffer);
		break;
	default:
		dump_descriptor(buffer, "\t\t");
	}
}

void
audio_midi_endpoint(usb_descriptor_t *desc)
{
	const uint8_t *buffer = (const uint8_t *)desc;
	uint8_t n, i;

	printf("\t\tMS Bulk Data Endpoint Descriptor:\n"
	       "\t\t  bLength            %d\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  bNumEmbMIDIJack    %d\n",
	       buffer[0], buffer[1], buffer[2], buffer[3]);

	n = buffer[3];

	for (i = 1 ; i <= n ; i++)
		printf("\t\t  baAssocJackID(%d)   %d\n", i, buffer[4+(i-1)]);

	printf("\n");
}

static void
audio_midi_header(const uint8_t *buffer)
{
	printf("\t\tMS Interface Header Descriptor:\n"
	       "\t\t  bLength            %d\n\t\t  bDescriptorType    %02x\n"
	       "\t\t  bDescriptorSubtype %02x\n\t\t  bcdMSC             %04x\n"
	       "\t\t  wTotalLength       %d\n\n",
	       buffer[0], buffer[1], buffer[2], UGETW(&buffer[3]),
	       UGETW(&buffer[5]));
}

static void
audio_midi_midi_in_jack(const uint8_t *buffer)
{
	printf("\t\tMIDI IN Jack Descriptor:\n\t\t  bLength            %d\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bJackType          %02x",
	       buffer[0], buffer[1], buffer[2], buffer[3]);

	switch (buffer[3]) {
	case USB_MIDI_EMBEDDED:
		printf(" (Embedded)\n");
		break;
	case USB_MIDI_EXTERNAL:
		printf(" (External)\n");
		break;
	default:
		printf(" (unknown)\n");
	}

	printf("\t\t  bJackID            %02x\n"
	       "\t\t  iJack              %d\n\n",
	       buffer[4], buffer[5]);
}

static void
audio_midi_midi_out_jack(const uint8_t *buffer)
{
	uint8_t p, i;

	printf("\t\tMIDI OUT Jack Descriptor:\n\t\t  bLength            %d\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubtype %02x\n"
	       "\t\t  bJackType          %02x",
	       buffer[0], buffer[1], buffer[2], buffer[3]);

	switch (buffer[3]) {
	case USB_MIDI_EMBEDDED:
		printf(" (Embedded)\n");
		break;
	case USB_MIDI_EXTERNAL:
		printf(" (External)\n");
		break;
	default:
		printf(" (unknown)\n");
	}

	printf("\t\t  bJackID            %02x\n\t\t  bNrInputPins       %d\n",
	       buffer[4], buffer[5]);

	p = buffer[5];

	for (i = 0 ; i < p ; i++) {
		printf("\t\t  baSourceID(%d)      %d\n"
		       "\t\t  BaSourcePin(%d)     %d\n",
		       i+1, buffer[6+2*i], i, buffer[6+2*i+1]);
	}

	printf("\t\t  iJack              %d\n\n", buffer[5+2*p]);
}

static void
audio_midi_element(const uint8_t *buffer)
{
	uint8_t p, n, i;

	printf("\t\tElement Descriptor:\n\t\t  bLength            %d\n"
	       "\t\t  bDescriptorType    %02x\n\t\t  bDescriptorSubType %02x\n"
	       "\t\t  bElementID         %02x\n\t\t  bNrInputPins       %d\n",
	       buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);

	p = buffer[4];

	for (i = 0 ; i < p ; i++) {
		printf("\t\t  baSourceID(%d)      %d\n"
		       "\t\t  BaSourcePin(%d)     %d\n",
		       i+1, buffer[5+2*i], i, buffer[5+2*i+1]);
	}

	printf("\t\t  bNrOutputPins      %d\n\t\t  bInTerminalLink    %02x\n"
	       "\t\t  bOutTerminalLink   %02x\n\t\t  bElCapsSize        %d\n",
	       buffer[5+2*p], buffer[6+2*p], buffer[7+2*p], buffer[8+2*p]);

	n = buffer[0] - 2 * p;

	for (i = 0 ; i < n ; i++)
		printf("\t\t  bmElementCaps      %02x\n", buffer[9+2*p+i]);

	printf("\t\t  iElement           %d\n\n", buffer[9+2*p+n]);
}
