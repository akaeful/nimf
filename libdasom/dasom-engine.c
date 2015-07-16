/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * dasom-engine.c
 * This file is part of Dasom.
 *
 * Copyright (C) 2015 Hodong Kim <hodong@cogno.org>
 *
 * Dasom is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Dasom is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program;  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dasom-engine.h"
#include "daemon/dasom-context.h"
#include "dasom-private.h"

enum
{
  PROP_0,
  PROP_PATH
};

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (DasomEngine, dasom_engine, G_TYPE_OBJECT);

static void
dasom_engine_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (object));

  DasomEngine *engine = DASOM_ENGINE (object);

  switch (prop_id)
  {
    case PROP_PATH:
      engine->priv->path = g_strdup (g_value_get_string (value));
      g_object_notify_by_pspec (object, pspec);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
dasom_engine_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (object));

  DasomEngine *engine = DASOM_ENGINE (object);

  switch (prop_id)
  {
    case PROP_PATH:
      g_value_set_string (value, engine->priv->path);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

void dasom_engine_reset (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (engine));

  DasomEngineClass *klass = DASOM_ENGINE_GET_CLASS (engine);

  if (klass->reset)
    klass->reset (engine);
}

void dasom_engine_focus_in (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (engine));

  DasomEngineClass *klass = DASOM_ENGINE_GET_CLASS (engine);

  if (klass->focus_in)
    klass->focus_in (engine);
}

void dasom_engine_focus_out (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (engine));

  DasomEngineClass *klass = DASOM_ENGINE_GET_CLASS (engine);

  if (klass->focus_out)
    klass->focus_out (engine);
}

gboolean dasom_engine_filter_event (DasomEngine      *engine,
                                    const DasomEvent *event)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  DasomEngineClass *klass = DASOM_ENGINE_GET_CLASS (engine);

  return klass->filter_event (engine, event);
}

gboolean dasom_engine_real_filter_event (DasomEngine      *engine,
                                         const DasomEvent *event)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  return FALSE;
}

void
dasom_engine_get_preedit_string (DasomEngine  *engine,
                                 gchar       **str,
                                 gint         *cursor_pos)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (engine));

  DasomEngineClass *klass = DASOM_ENGINE_GET_CLASS (engine);
  klass->get_preedit_string (engine, str, cursor_pos);

  g_return_if_fail (str == NULL || g_utf8_validate (*str, -1, NULL));
}

void
dasom_engine_set_surrounding (DasomEngine  *engine,
                              const char   *text,
                              gint          len,
                              gint          cursor_index)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_if_fail (DASOM_IS_ENGINE (engine));
  g_return_if_fail (text != NULL || len == 0);

  DasomEngineClass *class = DASOM_ENGINE_GET_CLASS (engine);

  if (class->set_surrounding)
    class->set_surrounding (engine, text, len, cursor_index);
}

gboolean
dasom_engine_get_surrounding (DasomEngine  *engine,
                              gchar       **text,
                              gint         *cursor_index)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gboolean retval = FALSE;
  DasomEngineClass *class = DASOM_ENGINE_GET_CLASS (engine);

  if (class->get_surrounding)
    retval = class->get_surrounding (engine, text, cursor_index);

  return retval;
}

void
dasom_engine_emit_preedit_start (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_signal_emit_by_name (engine->priv->daemon->target, "preedit-start");
}

void
dasom_engine_emit_preedit_changed (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_signal_emit_by_name (engine->priv->daemon->target, "preedit-changed");
}

void
dasom_engine_emit_preedit_end (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_signal_emit_by_name (engine->priv->daemon->target, "preedit-end");
}

void
dasom_engine_emit_commit (DasomEngine *engine, const gchar *text)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_signal_emit_by_name (engine->priv->daemon->target, "commit", text);
}

gboolean
dasom_engine_emit_delete_surrounding (DasomEngine *engine,
                                      gint         offset,
                                      gint         n_chars)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gboolean retval;
  g_signal_emit_by_name (engine->priv->daemon->target,
                         "delete-surrounding", offset, n_chars, &retval);
  return retval;
}

gboolean
dasom_engine_emit_retrieve_surrounding (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gboolean retval;
  g_signal_emit_by_name (engine->priv->daemon->target,
                         "retrieve-surrounding", &retval);
  return retval;
}

static void
dasom_engine_init (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  engine->priv = dasom_engine_get_instance_private (engine);
}

static void
dasom_engine_finalize (GObject *object)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  DasomEngine *engine = DASOM_ENGINE (object);

  g_free (engine->priv->path);
  g_free (engine->priv->surrounding_text);

  G_OBJECT_CLASS (dasom_engine_parent_class)->finalize (object);
}


static void
dasom_engine_real_get_preedit_string (DasomEngine  *engine,
                                      gchar       **str,
                                      gint         *cursor_pos)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  if (str)
    *str = g_strdup ("");

  if (cursor_pos)
    *cursor_pos = 0;
}

static void
dasom_engine_real_set_surrounding (DasomEngine  *engine,
                                   const char   *text,
                                   gint          len,
                                   gint          cursor_index)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_free (engine->priv->surrounding_text);
  engine->priv->surrounding_text         = g_strndup (text, len);
  engine->priv->surrounding_cursor_index = cursor_index;
}

static gboolean
dasom_engine_real_get_surrounding (DasomEngine  *engine,
                                   gchar       **text,
                                   gint         *cursor_index)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gboolean retval = dasom_engine_emit_retrieve_surrounding (engine);

  if (retval)
  {
    if (engine->priv->surrounding_text)
      *text = g_strdup (engine->priv->surrounding_text);
    else
      *text = g_strdup ("");

    *cursor_index = engine->priv->surrounding_cursor_index;
  }
  else
  {
    *text = NULL;
    *cursor_index = 0;
  }

  return retval;
}

const gchar *
dasom_engine_get_name (DasomEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  DasomEngineClass *klass = DASOM_ENGINE_GET_CLASS (engine);

  return klass->get_name (engine);
}

static const gchar *
dasom_engine_real_get_name (DasomEngine *engine)
{
  g_error ("You should implement your_engine_get_name ()");

  return NULL;
}

static void
dasom_engine_class_init (DasomEngineClass *klass)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize     = dasom_engine_finalize;

  object_class->set_property = dasom_engine_set_property;
  object_class->get_property = dasom_engine_get_property;

  klass->filter_event        = dasom_engine_real_filter_event;
  klass->get_preedit_string  = dasom_engine_real_get_preedit_string;
  klass->set_surrounding     = dasom_engine_real_set_surrounding;
  klass->get_surrounding     = dasom_engine_real_get_surrounding;
 /* FIXME: 나중에  get_engine_info 이런 걸로 추가해야 할지도 모르겠습니다. */
  klass->get_name            = dasom_engine_real_get_name;

  g_object_class_install_property (object_class,
                                   PROP_PATH,
                                   g_param_spec_string ("path",
                                                        "path",
                                                        "path",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}
