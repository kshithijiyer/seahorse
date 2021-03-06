/*
 * Seahorse
 *
 * Copyright (C) 2005 Stefan Walter
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

#include "seahorse-gpgme-expires-dialog.h"
#include "seahorse-gpgme-key-op.h"
#include "seahorse-gpgme-subkey.h"

#include "libseahorse/seahorse-util.h"

#include <glib/gi18n.h>

#include <string.h>

struct _SeahorseGpgmeExpiresDialog {
    GtkDialog parent_instance;

    SeahorseGpgmeSubkey *subkey;

    GtkWidget *calendar;
    GtkWidget *never_expires_check;
};

enum {
    PROP_0,
    PROP_SUBKEY,
    N_PROPS
};
static GParamSpec *obj_props[N_PROPS] = { NULL, };

G_DEFINE_TYPE (SeahorseGpgmeExpiresDialog, seahorse_gpgme_expires_dialog, GTK_TYPE_DIALOG)

static void
seahorse_gpgme_expires_dialog_response (GtkDialog *dialog, int response)
{
    SeahorseGpgmeExpiresDialog *self = SEAHORSE_GPGME_EXPIRES_DIALOG (dialog);
    gpgme_error_t err;
    time_t expiry = 0;

    if (response != GTK_RESPONSE_OK)
        return;

    if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->never_expires_check))) {
        struct tm when;

        memset (&when, 0, sizeof (when));
        gtk_calendar_get_date (GTK_CALENDAR (self->calendar),
                               (guint*) &(when.tm_year),
                               (guint*) &(when.tm_mon),
                               (guint*) &(when.tm_mday));
        when.tm_year -= 1900;
        expiry = mktime (&when);

        if (expiry <= time (NULL)) {
            seahorse_util_show_error (self->calendar, _("Invalid expiry date"),
                                      _("The expiry date must be in the future"));
            return;
        }
    }

    gtk_widget_set_sensitive (gtk_dialog_get_content_area (GTK_DIALOG (self)), FALSE);

    if (expiry != (time_t)seahorse_pgp_subkey_get_expires (SEAHORSE_PGP_SUBKEY (self->subkey))) {
        err = seahorse_gpgme_key_op_set_expires (self->subkey, expiry);
        if (!GPG_IS_OK (err))
            seahorse_gpgme_handle_error (err, _("Couldn’t change expiry date"));
    }
}

static void
on_gpgme_expire_toggled (GtkWidget *widget,
                         gpointer user_data)
{
    SeahorseGpgmeExpiresDialog *self = SEAHORSE_GPGME_EXPIRES_DIALOG (user_data);

    gtk_widget_set_sensitive (self->calendar,
                              !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->never_expires_check)));
}

static void
seahorse_gpgme_expires_dialog_get_property (GObject *object,
                                            guint prop_id,
                                            GValue *value,
                                            GParamSpec *pspec)
{
    SeahorseGpgmeExpiresDialog *self = SEAHORSE_GPGME_EXPIRES_DIALOG (object);

    switch (prop_id) {
    case PROP_SUBKEY:
        g_value_set_object (value, self->subkey);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
seahorse_gpgme_expires_dialog_set_property (GObject *object,
                                            guint prop_id,
                                            const GValue *value,
                                            GParamSpec *pspec)
{
    SeahorseGpgmeExpiresDialog *self = SEAHORSE_GPGME_EXPIRES_DIALOG (object);

    switch (prop_id) {
    case PROP_SUBKEY:
        g_clear_object (&self->subkey);
        self->subkey = g_value_dup_object (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
seahorse_gpgme_expires_dialog_finalize (GObject *obj)
{
    SeahorseGpgmeExpiresDialog *self = SEAHORSE_GPGME_EXPIRES_DIALOG (obj);

    g_clear_object (&self->subkey);

    G_OBJECT_CLASS (seahorse_gpgme_expires_dialog_parent_class)->finalize (obj);
}

static void
seahorse_gpgme_expires_dialog_constructed (GObject *obj)
{
    SeahorseGpgmeExpiresDialog *self = SEAHORSE_GPGME_EXPIRES_DIALOG (obj);
    g_autofree char *title = NULL;
    const char *label;
    gulong expires;

    G_OBJECT_CLASS (seahorse_gpgme_expires_dialog_parent_class)->constructed (obj);

    label = seahorse_pgp_subkey_get_description (SEAHORSE_PGP_SUBKEY (self->subkey));
    title = g_strdup_printf (_("Expiry: %s"), label);
    gtk_window_set_title (GTK_WINDOW (self), title);

    expires = seahorse_pgp_subkey_get_expires (SEAHORSE_PGP_SUBKEY (self->subkey));
    if (expires == 0) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->never_expires_check), TRUE);
        gtk_widget_set_sensitive (self->calendar, FALSE);
    } else {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->never_expires_check), FALSE);
        gtk_widget_set_sensitive (self->calendar, TRUE);
    }

    if (expires) {
        struct tm t;
        time_t time = (time_t)expires;
        if (gmtime_r (&time, &t)) {
            gtk_calendar_select_month (GTK_CALENDAR (self->calendar), t.tm_mon, t.tm_year + 1900);
            gtk_calendar_select_day (GTK_CALENDAR (self->calendar), t.tm_mday);
        }
    }
}

static void
seahorse_gpgme_expires_dialog_init (SeahorseGpgmeExpiresDialog *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));
}

static void
seahorse_gpgme_expires_dialog_class_init (SeahorseGpgmeExpiresDialogClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    GtkDialogClass *dialog_class = GTK_DIALOG_CLASS (klass);

    gobject_class->constructed = seahorse_gpgme_expires_dialog_constructed;
    gobject_class->get_property = seahorse_gpgme_expires_dialog_get_property;
    gobject_class->set_property = seahorse_gpgme_expires_dialog_set_property;
    gobject_class->finalize = seahorse_gpgme_expires_dialog_finalize;

    obj_props[PROP_SUBKEY] =
        g_param_spec_object ("subkey", "Subkey",
                             "Subkey",
                             SEAHORSE_GPGME_TYPE_SUBKEY,
                             G_PARAM_READWRITE |
                             G_PARAM_CONSTRUCT_ONLY |
                             G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (gobject_class, N_PROPS, obj_props);

    gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Seahorse/seahorse-gpgme-expires-dialog.ui");
    gtk_widget_class_bind_template_child (widget_class, SeahorseGpgmeExpiresDialog, calendar);
    gtk_widget_class_bind_template_child (widget_class, SeahorseGpgmeExpiresDialog, never_expires_check);
    gtk_widget_class_bind_template_callback (widget_class, on_gpgme_expire_toggled);

    dialog_class->response = seahorse_gpgme_expires_dialog_response;
}

GtkDialog *
seahorse_gpgme_expires_dialog_new (SeahorseGpgmeSubkey *subkey,
                                   GtkWindow *parent)
{
    g_return_val_if_fail (SEAHORSE_GPGME_IS_SUBKEY (subkey), NULL);

    return g_object_new (SEAHORSE_GPGME_TYPE_EXPIRES_DIALOG,
                         "subkey", subkey,
                         "use-header-bar", 1,
                         NULL);
}
