/*
 * Seahorse
 *
 * Copyright (C) 2008 Stefan Walter
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <glib-object.h>

#include "seahorse-common.h"

#include "seahorse-pgp-key.h"

#define SEAHORSE_PGP_TYPE_UID            (seahorse_pgp_uid_get_type ())

#define SEAHORSE_PGP_UID(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SEAHORSE_PGP_TYPE_UID, SeahorsePgpUid))
#define SEAHORSE_PGP_UID_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SEAHORSE_PGP_TYPE_UID, SeahorsePgpUidClass))
#define SEAHORSE_PGP_IS_UID(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SEAHORSE_PGP_TYPE_UID))
#define SEAHORSE_PGP_IS_UID_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SEAHORSE_PGP_TYPE_UID))
#define SEAHORSE_PGP_UID_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), SEAHORSE_PGP_TYPE_UID, SeahorsePgpUidClass))

typedef struct _SeahorsePgpUid SeahorsePgpUid;
typedef struct _SeahorsePgpUidClass SeahorsePgpUidClass;
typedef struct _SeahorsePgpUidPrivate SeahorsePgpUidPrivate;

struct _SeahorsePgpUid {
	SeahorseObject parent;
};

struct _SeahorsePgpUidClass {
	SeahorseObjectClass parent_class;
};

G_DEFINE_AUTOPTR_CLEANUP_FUNC (SeahorsePgpUid, g_object_unref)

GType             seahorse_pgp_uid_get_type             (void);

SeahorsePgpUid*   seahorse_pgp_uid_new                  (SeahorsePgpKey *parent,
                                                         const gchar *uid_string);

SeahorsePgpKey *  seahorse_pgp_uid_get_parent           (SeahorsePgpUid *self);

void              seahorse_pgp_uid_realize              (SeahorsePgpUid *self);

GList*            seahorse_pgp_uid_get_signatures       (SeahorsePgpUid *self);

void              seahorse_pgp_uid_set_signatures       (SeahorsePgpUid *self,
                                                         GList *signatures);

SeahorseValidity  seahorse_pgp_uid_get_validity         (SeahorsePgpUid *self);

void              seahorse_pgp_uid_set_validity         (SeahorsePgpUid *self,
                                                         SeahorseValidity validity);

const gchar*      seahorse_pgp_uid_get_name             (SeahorsePgpUid *self);

void              seahorse_pgp_uid_set_name             (SeahorsePgpUid *self,
                                                         const gchar *name);

const gchar*      seahorse_pgp_uid_get_email            (SeahorsePgpUid *self);

void              seahorse_pgp_uid_set_email            (SeahorsePgpUid *self,
                                                         const gchar *comment);

const gchar*      seahorse_pgp_uid_get_comment          (SeahorsePgpUid *self);

void              seahorse_pgp_uid_set_comment          (SeahorsePgpUid *self,
                                                         const gchar *comment);

gchar*            seahorse_pgp_uid_calc_label           (const gchar *name,
                                                         const gchar *email,
                                                         const gchar *comment);

gchar*            seahorse_pgp_uid_calc_markup          (const gchar *name,
                                                         const gchar *email,
                                                         const gchar *comment,
                                                         guint flags);

GQuark            seahorse_pgp_uid_calc_id              (GQuark key_id,
                                                         guint index);
