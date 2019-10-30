/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 *
 * Mapping MIDI CC to controls.
 */

#ifndef __AUDIO_MIDI_MAPPING_H__
#define __AUDIO_MIDI_MAPPING_H__

#include "audio/ext_port.h"
#include "audio/midi.h"
#include "audio/port.h"

/**
 * @addtogroup audio
 *
 * @{
 */

#define MIDI_MAPPINGS (AUDIO_ENGINE->midi_mappings)

/**
 * A mapping from a MIDI value to a destination.
 */
typedef struct MidiMapping
{
  /** Raw MIDI signal. */
  midi_byte_t    key[3];

  /** The device that this connection will be mapped
   * for. */
  ExtPort *      device_port;

  /** Destination. */
  PortIdentifier dest_id;

  /** Destination pointer, for convenience. */
  Port *         dest;
} MidiMapping;

/**
 * All MIDI mappings in Zrythm.
 */
typedef struct MidiMappings
{
  MidiMapping     mappings[1028];
  int             num_mappings;
} MidiMappings;

static const cyaml_schema_field_t
midi_mapping_fields_schema[] =
{
  CYAML_FIELD_SEQUENCE_FIXED (
    "key", CYAML_FLAG_DEFAULT,
    MidiMapping, key,
    &uint8_t_schema, 3),
  CYAML_FIELD_MAPPING_PTR (
    "device_port", CYAML_FLAG_POINTER,
    MidiMapping, device_port,
    ext_port_fields_schema),
  CYAML_FIELD_MAPPING (
    "dest_id", CYAML_FLAG_DEFAULT,
    MidiMapping, dest_id,
    port_identifier_fields_schema),

	CYAML_FIELD_END
};

static const cyaml_schema_value_t
midi_mapping_schema =
{
	CYAML_VALUE_MAPPING (
    CYAML_FLAG_POINTER,
	  MidiMapping, midi_mapping_fields_schema),
};

static const cyaml_schema_field_t
midi_mappings_fields_schema[] =
{
  CYAML_FIELD_SEQUENCE_COUNT (
    "mappings", CYAML_FLAG_DEFAULT,
    MidiMappings, mappings, num_mappings,
    &midi_mapping_schema,
    0, CYAML_UNLIMITED),

	CYAML_FIELD_END
};

static const cyaml_schema_value_t
midi_mappings_schema =
{
	CYAML_VALUE_MAPPING (
    CYAML_FLAG_POINTER,
	  MidiMappings, midi_mappings_fields_schema),
};

/**
 * Initializes the MidiMappings after a Project
 * is loaded.
 */
void
midi_mappings_init_loaded (
  MidiMappings * self);

/**
 * Returns a newly allocated MidiMappings.
 */
MidiMappings *
midi_mappings_new (void);

/**
 * Binds the CC represented by the given raw buffer
 * (must be size 3) to the given Port.
 */
void
midi_mappings_bind (
  MidiMappings * self,
  midi_byte_t *  buf,
  ExtPort *      device_port,
  Port *         dest_port);

/*
 * @}
 */

#endif