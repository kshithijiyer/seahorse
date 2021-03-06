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

#include "config.h"

#include "seahorse-gpgme.h"
#include "seahorse-pgp-subkey.h"
#include "seahorse-pgp-uid.h"

#include <string.h>

#include <glib/gi18n.h>

enum {
    PROP_0,
    PROP_INDEX,
    PROP_KEYID,
    PROP_FLAGS,
    PROP_LENGTH,
    PROP_ALGORITHM,
    PROP_CREATED,
    PROP_EXPIRES,
    PROP_DESCRIPTION,
    PROP_FINGERPRINT,
    N_PROPS
};
static GParamSpec *obj_props[N_PROPS] = { NULL, };

typedef struct _SeahorsePgpSubkeyPrivate {
    guint index;
    gchar *keyid;
    guint flags;
    guint length;
    gchar *algorithm;
    gulong created;
    gulong expires;
    gchar *description;
    gchar *fingerprint;
} SeahorsePgpSubkeyPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (SeahorsePgpSubkey, seahorse_pgp_subkey, G_TYPE_OBJECT);

guint
seahorse_pgp_subkey_get_index (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), 0);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->index;
}

void
seahorse_pgp_subkey_set_index (SeahorsePgpSubkey *self, guint index)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    priv->index = index;
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_INDEX]);
}

const gchar*
seahorse_pgp_subkey_get_keyid (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), NULL);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->keyid;
}

void
seahorse_pgp_subkey_set_keyid (SeahorsePgpSubkey *self, const gchar *keyid)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    g_free (priv->keyid);
    priv->keyid = g_strdup (keyid);
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_KEYID]);
}

guint
seahorse_pgp_subkey_get_flags (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), 0);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->flags;
}

void
seahorse_pgp_subkey_set_flags (SeahorsePgpSubkey *self, guint flags)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    priv->flags = flags;
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_FLAGS]);
}

guint
seahorse_pgp_subkey_get_length (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), 0);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->length;
}

void
seahorse_pgp_subkey_set_length (SeahorsePgpSubkey *self, guint length)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    priv->length = length;
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_LENGTH]);
}

gchar *
seahorse_pgp_subkey_get_usage (SeahorsePgpSubkey *self)
{
    typedef struct {
        unsigned int flag;
        const char *name;
    } FlagNames;

    const FlagNames flag_names[] = {
        { SEAHORSE_FLAG_CAN_ENCRYPT,      N_("Encrypt") },
        { SEAHORSE_FLAG_CAN_SIGN,         N_("Sign") },
        { SEAHORSE_FLAG_CAN_CERTIFY,      N_("Certify") },
        { SEAHORSE_FLAG_CAN_AUTHENTICATE, N_("Authenticate") }
    };

    SeahorsePgpSubkeyPrivate *priv;
    GString *str;
    gboolean previous;
    uint i;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), NULL);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    str = g_string_new (NULL);
    previous = FALSE;

    for (i = 0; i < G_N_ELEMENTS (flag_names); i++) {
        if (priv->flags & flag_names[i].flag) {
            if (previous)
                g_string_append (str, ", ");

            previous = TRUE;
            g_string_append (str, _(flag_names[i].name));
        }
    }

    return g_string_free (str, FALSE);
}

const gchar*
seahorse_pgp_subkey_get_algorithm (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), NULL);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->algorithm;
}

void
seahorse_pgp_subkey_set_algorithm (SeahorsePgpSubkey *self, const gchar *algorithm)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    g_free (priv->algorithm);
    priv->algorithm = g_strdup (algorithm);
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ALGORITHM]);
}

gulong
seahorse_pgp_subkey_get_created (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), 0);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->created;
}

void
seahorse_pgp_subkey_set_created (SeahorsePgpSubkey *self, gulong created)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    priv->created = created;
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_CREATED]);
}

gulong
seahorse_pgp_subkey_get_expires (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), 0);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->expires;
}

void
seahorse_pgp_subkey_set_expires (SeahorsePgpSubkey *self, gulong expires)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    priv->expires = expires;
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_EXPIRES]);
}

const gchar*
seahorse_pgp_subkey_get_fingerprint (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), NULL);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->fingerprint;
}

void
seahorse_pgp_subkey_set_fingerprint (SeahorsePgpSubkey *self, const gchar *fingerprint)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    g_free (priv->fingerprint);
    priv->fingerprint = g_strdup (fingerprint);
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_FINGERPRINT]);
}

const gchar*
seahorse_pgp_subkey_get_description (SeahorsePgpSubkey *self)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_val_if_fail (SEAHORSE_PGP_IS_SUBKEY (self), NULL);

    priv = seahorse_pgp_subkey_get_instance_private (self);
    return priv->description;
}

void
seahorse_pgp_subkey_set_description (SeahorsePgpSubkey *self, const gchar *description)
{
    SeahorsePgpSubkeyPrivate *priv;

    g_return_if_fail (SEAHORSE_PGP_IS_SUBKEY (self));

    priv = seahorse_pgp_subkey_get_instance_private (self);
    g_free (priv->description);
    priv->description = g_strdup (description);
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_DESCRIPTION]);
}

gchar*
seahorse_pgp_subkey_calc_description (const gchar *name, guint index)
{
    if (name == NULL)
        name = _("Key");

    if (index == 0)
        return g_strdup (name);

    return g_strdup_printf (_("Subkey %d of %s"), index, name);
}

/* Takes runs of hexadecimal digits, possibly with whitespace among them, and
 * formats them nicely in groups of four digits.
 */
gchar*
seahorse_pgp_subkey_calc_fingerprint (const gchar *raw_fingerprint)
{
    const gchar *raw;
    GString *string;
    guint i, len;
    guint num_digits;
    gchar *fpr;

    raw = raw_fingerprint;
    g_return_val_if_fail (raw != NULL, NULL);

    string = g_string_new ("");
    len = strlen (raw);

    num_digits = 0;
    for (i = 0; i < len; i++) {
        if (g_ascii_isxdigit (raw[i])) {
            g_string_append_c (string, g_ascii_toupper (raw[i]));
            num_digits++;

            if (num_digits > 0 && num_digits % 4 == 0)
                g_string_append (string, " ");
        }
    }

    fpr = string->str;
    g_string_free (string, FALSE);

    g_strchomp (fpr);

    return fpr;
}

static void
seahorse_pgp_subkey_get_property (GObject *object, guint prop_id,
                                  GValue *value, GParamSpec *pspec)
{
    SeahorsePgpSubkey *self = SEAHORSE_PGP_SUBKEY (object);

    switch (prop_id) {
    case PROP_INDEX:
        g_value_set_uint (value, seahorse_pgp_subkey_get_index (self));
        break;
    case PROP_KEYID:
        g_value_set_string (value, seahorse_pgp_subkey_get_keyid (self));
        break;
    case PROP_FLAGS:
        g_value_set_uint (value, seahorse_pgp_subkey_get_flags (self));
        break;
    case PROP_LENGTH:
        g_value_set_uint (value, seahorse_pgp_subkey_get_length (self));
        break;
    case PROP_ALGORITHM:
        g_value_set_string (value, seahorse_pgp_subkey_get_algorithm (self));
        break;
    case PROP_CREATED:
        g_value_set_ulong (value, seahorse_pgp_subkey_get_created (self));
        break;
    case PROP_EXPIRES:
        g_value_set_ulong (value, seahorse_pgp_subkey_get_expires (self));
        break;
    case PROP_DESCRIPTION:
        g_value_set_string (value, seahorse_pgp_subkey_get_description (self));
        break;
    case PROP_FINGERPRINT:
        g_value_set_string (value, seahorse_pgp_subkey_get_fingerprint (self));
        break;
    }
}

static void
seahorse_pgp_subkey_set_property (GObject *object, guint prop_id, const GValue *value,
                                  GParamSpec *pspec)
{
    SeahorsePgpSubkey *self = SEAHORSE_PGP_SUBKEY (object);

    switch (prop_id) {
    case PROP_INDEX:
        seahorse_pgp_subkey_set_index (self, g_value_get_uint (value));
        break;
    case PROP_KEYID:
        seahorse_pgp_subkey_set_keyid (self, g_value_get_string (value));
        break;
    case PROP_FLAGS:
        seahorse_pgp_subkey_set_flags (self, g_value_get_uint (value));
        break;
    case PROP_LENGTH:
        seahorse_pgp_subkey_set_length (self, g_value_get_uint (value));
        break;
    case PROP_ALGORITHM:
        seahorse_pgp_subkey_set_algorithm (self, g_value_get_string (value));
        break;
    case PROP_CREATED:
        seahorse_pgp_subkey_set_created (self, g_value_get_ulong (value));
        break;
    case PROP_EXPIRES:
        seahorse_pgp_subkey_set_expires (self, g_value_get_ulong (value));
        break;
    case PROP_FINGERPRINT:
        seahorse_pgp_subkey_set_fingerprint (self, g_value_get_string (value));
        break;
    case PROP_DESCRIPTION:
        seahorse_pgp_subkey_set_description (self, g_value_get_string (value));
        break;
    }
}

static void
seahorse_pgp_subkey_finalize (GObject *gobject)
{
    SeahorsePgpSubkey *self = SEAHORSE_PGP_SUBKEY (gobject);
    SeahorsePgpSubkeyPrivate *priv
        = seahorse_pgp_subkey_get_instance_private (self);

    g_clear_pointer (&priv->algorithm, g_free);
    g_clear_pointer (&priv->fingerprint, g_free);
    g_clear_pointer (&priv->description, g_free);
    g_clear_pointer (&priv->keyid, g_free);

    G_OBJECT_CLASS (seahorse_pgp_subkey_parent_class)->finalize (gobject);
}

static void
seahorse_pgp_subkey_init (SeahorsePgpSubkey *self)
{
}

static void
seahorse_pgp_subkey_class_init (SeahorsePgpSubkeyClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = seahorse_pgp_subkey_finalize;
    gobject_class->set_property = seahorse_pgp_subkey_set_property;
    gobject_class->get_property = seahorse_pgp_subkey_get_property;

    obj_props[PROP_INDEX] =
        g_param_spec_uint ("index", "Index", "PGP subkey index",
                           0, G_MAXUINT, 0,
                           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_KEYID] =
        g_param_spec_string ("keyid", "Key ID", "GPG Key ID",
                             "",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_FLAGS] =
        g_param_spec_uint ("flags", "Flags", "PGP subkey flags",
                           0, G_MAXUINT, 0,
                           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_LENGTH] =
        g_param_spec_uint ("length", "Length", "PGP key length",
                           0, G_MAXUINT, 0,
                           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_ALGORITHM] =
        g_param_spec_string ("algorithm", "Algorithm", "GPG Algorithm",
                             "",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_CREATED] =
        g_param_spec_ulong ("created", "Created On", "Date this key was created on",
                            0, G_MAXULONG, 0,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_EXPIRES] =
        g_param_spec_ulong ("expires", "Expires On", "Date this key expires on",
                            0, G_MAXULONG, 0,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_DESCRIPTION] =
        g_param_spec_string ("description", "Description", "Key Description",
                             "",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_props[PROP_FINGERPRINT] =
        g_param_spec_string ("fingerprint", "Fingerprint", "PGP Key Fingerprint",
                             "",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (gobject_class, N_PROPS, obj_props);
}

SeahorsePgpSubkey*
seahorse_pgp_subkey_new (void)
{
    return g_object_new (SEAHORSE_PGP_TYPE_SUBKEY, NULL);
}
