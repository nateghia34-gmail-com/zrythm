/*
 * gui/widgets/chord_track.h - ChordTrack view
 *
 * Copyright (C) 2018 Alexandros Theodotou
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __GUI_WIDGETS_CHORD_TRACK_H__
#define __GUI_WIDGETS_CHORD_TRACK_H__

#include "audio/channel.h"

#include <gtk/gtk.h>

#define CHORD_TRACK_WIDGET_TYPE                  (chord_track_widget_get_type ())
G_DECLARE_FINAL_TYPE (ChordTrackWidget,
                      chord_track_widget,
                      CHORD_TRACK,
                      WIDGET,
                      GtkPaned)

typedef struct ChordTrack ChordTrack;
typedef struct ChordTracklistWidget ChordTracklistWidget;

typedef struct _ChordTrackWidget
{
  GtkPaned                      parent_instance;
  TrackWidget *                 parent;
  GtkBox *                      track_box;
  GtkGrid *                     track_grid;
  GtkLabel *                    track_name;
  GtkButton *                   record;
  GtkButton *                   solo;
  GtkButton *                   mute;
  GtkImage *                    icon;

  /**
   * For showing/hiding additional sub-tracks relating to
   * chords.
   */
  ChordTracklistWidget *        chord_tracklist_widget;
} ChordTrackWidget;

/**
 * Creates a new chord_track widget from the given chord_track.
 */
ChordTrackWidget *
chord_track_widget_new (ChordTrack * chord_track,
                        TrackWidget * parent);

/**
 * Updates changes in the backend to the ui
 */
void
chord_track_widget_update_all (ChordTrackWidget * self);

/**
 * Makes sure the chord_track widget and its elements have the visibility they should.
 */
void
chord_track_widget_show (ChordTrackWidget * self);

#endif
