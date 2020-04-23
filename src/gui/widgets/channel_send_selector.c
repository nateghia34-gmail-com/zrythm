/*
 * Copyright (C) 2020 Alexandros Theodotou <alex at zrythm dot org>
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

#include "audio/channel_send.h"
#include "audio/track.h"
#include "audio/tracklist.h"
#include "plugins/plugin_identifier.h"
#include "gui/widgets/channel_send.h"
#include "gui/widgets/channel_send_selector.h"
#include "project.h"

#include <glib/gi18n.h>

G_DEFINE_TYPE (
  ChannelSendSelectorWidget,
  channel_send_selector_widget,
  GTK_TYPE_POPOVER)

/**
 * Target type.
 */
typedef enum ChannelSendTargetType
{
  /** Remove send. */
  TARGET_TYPE_NONE,

  /** Send to track inputs. */
  TARGET_TYPE_TRACK,

  /** Send to plugin sidechain inputs. */
  TARGET_TYPE_PLUGIN_SIDECHAIN,
} ChannelSendTargetType;

typedef struct ChannelSendTarget
{
  ChannelSendTargetType type;

  int                   track_pos;
  PluginIdentifier      pl_id;
} ChannelSendTarget;

static void
on_ok_clicked (
  GtkButton * btn,
  ChannelSendSelectorWidget * self)
{
  gtk_widget_destroy (GTK_WIDGET (self));
}

static void
setup_treeview (
  ChannelSendSelectorWidget * self)
{
  /* icon, name, pointer to data */
  GtkListStore * list_store =
    gtk_list_store_new (
      3, G_TYPE_STRING, G_TYPE_STRING,
      G_TYPE_POINTER);

  GtkTreeIter iter;
  gtk_list_store_append (list_store, &iter);
  gtk_list_store_set (
    list_store, &iter,
    0, "z-folder",
    1, _("None"),
    2, NULL,
    -1);

  Track * track =
    channel_send_get_track (self->send_widget->send);
  for (int i = 0; i < TRACKLIST->num_tracks; i++)
    {
      Track * target_track = TRACKLIST->tracks[i];

      g_message ("target %s", target_track->name);

      /* skip tracks with non-matching signal types */
      if (target_track == track ||
          track->out_signal_type !=
            target_track->in_signal_type ||
          !track_type_is_fx (target_track->type))
        continue;
      g_message ("adding %s", target_track->name);

      /* create target */
      ChannelSendTarget * target =
        calloc (1, sizeof (ChannelSendTarget));
      target->type = TARGET_TYPE_TRACK;
      target->track_pos = i;

      /* add it to list */
      gtk_list_store_append (list_store, &iter);
      gtk_list_store_set (
        list_store, &iter,
        0, "z-folder",
        1, target_track->name,
        2, target,
        -1);
    }

  /* TODO plugin sidechain inputs */

  self->model = GTK_TREE_MODEL (list_store);
  gtk_tree_view_set_model (
    self->treeview, self->model);
}

ChannelSendSelectorWidget *
channel_send_selector_widget_new (
  ChannelSendWidget * send)
{
  ChannelSendSelectorWidget * self =
    g_object_new (
      CHANNEL_SEND_SELECTOR_WIDGET_TYPE,
      "relative-to", GTK_WIDGET (send),
      NULL);
  self->send_widget = send;


  setup_treeview (self);

  return self;
}

static void
channel_send_selector_widget_class_init (
  ChannelSendSelectorWidgetClass * klass)
{
}

static void
channel_send_selector_widget_init (
  ChannelSendSelectorWidget * self)
{
  /* create box */
  self->vbox =
    GTK_BOX (
      gtk_box_new (GTK_ORIENTATION_VERTICAL, 0));
  gtk_widget_set_visible (
    GTK_WIDGET (self->vbox), TRUE);
  gtk_container_add (
    GTK_CONTAINER (self),
    GTK_WIDGET (self->vbox));

  /* add treeview */
  self->treeview =
    GTK_TREE_VIEW (gtk_tree_view_new ());
  gtk_widget_set_visible (
    GTK_WIDGET (self->treeview), TRUE);
  gtk_container_add (
    GTK_CONTAINER (self->vbox),
    GTK_WIDGET (self->treeview));

  /* add button group */
  self->btn_box =
    GTK_BUTTON_BOX (
      gtk_button_box_new (
        GTK_ORIENTATION_HORIZONTAL));
  gtk_widget_set_visible (
    GTK_WIDGET (self->btn_box), TRUE);
  gtk_container_add (
    GTK_CONTAINER (self->vbox),
    GTK_WIDGET (self->btn_box));
  self->ok_btn =
    GTK_BUTTON (gtk_button_new_with_label (_("OK")));
  gtk_widget_set_visible (
    GTK_WIDGET (self->ok_btn), TRUE);
  gtk_container_add (
    GTK_CONTAINER (self->btn_box),
    GTK_WIDGET (self->ok_btn));

  /* init tree view */
  GtkCellRenderer * renderer;
  GtkTreeViewColumn * column;

  /* column for icon */
  renderer = gtk_cell_renderer_pixbuf_new ();
  column =
    gtk_tree_view_column_new_with_attributes (
      "icon", renderer,
      "icon-name", 0,
      NULL);
  gtk_tree_view_append_column (
    self->treeview, column);

  /* column for name */
  renderer = gtk_cell_renderer_text_new ();
  column =
    gtk_tree_view_column_new_with_attributes (
      "name", renderer,
      "text", 1,
      NULL);
  gtk_tree_view_append_column (
    self->treeview, column);

  /* set search column */
  gtk_tree_view_set_search_column (
    self->treeview, 1);

  /* set headers invisible */
  gtk_tree_view_set_headers_visible (
    self->treeview, false);

#if 0
  g_signal_connect (
    G_OBJECT (
      gtk_tree_view_get_selection (
        GTK_TREE_VIEW (self->treeview))), "changed",
     G_CALLBACK (on_selection_changed), self);
#endif
  g_signal_connect (
    G_OBJECT (self->ok_btn), "clicked",
    G_CALLBACK (on_ok_clicked), self);
}