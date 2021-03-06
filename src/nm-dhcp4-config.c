// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2008 Red Hat, Inc.
 */

#include "nm-default.h"

#include "nm-dhcp4-config.h"

#include "nm-dbus-interface.h"
#include "nm-utils.h"
#include "nm-dbus-object.h"
#include "nm-core-utils.h"

/*****************************************************************************/

NM_GOBJECT_PROPERTIES_DEFINE (NMDhcp4Config,
	PROP_OPTIONS,
);

typedef struct {
	GVariant *options;
} NMDhcp4ConfigPrivate;

struct _NMDhcp4Config {
	NMDBusObject parent;
	NMDhcp4ConfigPrivate _priv;
};

struct _NMDhcp4ConfigClass {
	NMDBusObjectClass parent;
};

G_DEFINE_TYPE (NMDhcp4Config, nm_dhcp4_config, NM_TYPE_DBUS_OBJECT)

#define NM_DHCP4_CONFIG_GET_PRIVATE(self) _NM_GET_PRIVATE (self, NMDhcp4Config, NM_IS_DHCP4_CONFIG)

/*****************************************************************************/

void
nm_dhcp4_config_set_options (NMDhcp4Config *self,
                             GHashTable *options)
{
	NMDhcp4ConfigPrivate *priv = NM_DHCP4_CONFIG_GET_PRIVATE (self);
	GVariant *val;

	g_return_if_fail (NM_IS_DHCP4_CONFIG (self));
	g_return_if_fail (options);

	val = nm_utils_strdict_to_variant (options);
	g_variant_unref (priv->options);
	priv->options = g_variant_ref_sink (val);
	_notify (self, PROP_OPTIONS);
}

const char *
nm_dhcp4_config_get_option (NMDhcp4Config *self, const char *key)
{
	NMDhcp4ConfigPrivate *priv = NM_DHCP4_CONFIG_GET_PRIVATE (self);
	const char *value;

	g_return_val_if_fail (NM_IS_DHCP4_CONFIG (self), NULL);
	g_return_val_if_fail (key != NULL, NULL);

	if (g_variant_lookup (priv->options, key, "&s", &value))
		return value;
	else
		return NULL;
}

GVariant *
nm_dhcp4_config_get_options (NMDhcp4Config *self)
{
	g_return_val_if_fail (NM_IS_DHCP4_CONFIG (self), NULL);

	return g_variant_ref (NM_DHCP4_CONFIG_GET_PRIVATE (self)->options);
}

/*****************************************************************************/

static void
get_property (GObject *object, guint prop_id,
              GValue *value, GParamSpec *pspec)
{
	NMDhcp4ConfigPrivate *priv = NM_DHCP4_CONFIG_GET_PRIVATE (object);

	switch (prop_id) {
	case PROP_OPTIONS:
		g_value_set_variant (value, priv->options);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/*****************************************************************************/

static void
nm_dhcp4_config_init (NMDhcp4Config *self)
{
	NMDhcp4ConfigPrivate *priv = NM_DHCP4_CONFIG_GET_PRIVATE (self);

	priv->options = g_variant_new_array (G_VARIANT_TYPE ("{sv}"), NULL, 0);
	g_variant_ref_sink (priv->options);
}

NMDhcp4Config *
nm_dhcp4_config_new (void)
{
	return NM_DHCP4_CONFIG (g_object_new (NM_TYPE_DHCP4_CONFIG, NULL));
}

static void
finalize (GObject *object)
{
	NMDhcp4ConfigPrivate *priv = NM_DHCP4_CONFIG_GET_PRIVATE (object);

	g_variant_unref (priv->options);

	G_OBJECT_CLASS (nm_dhcp4_config_parent_class)->finalize (object);
}

static const NMDBusInterfaceInfoExtended interface_info_dhcp4_config = {
	.parent = NM_DEFINE_GDBUS_INTERFACE_INFO_INIT (
		NM_DBUS_INTERFACE_DHCP4_CONFIG,
		.signals = NM_DEFINE_GDBUS_SIGNAL_INFOS (
			&nm_signal_info_property_changed_legacy,
		),
		.properties = NM_DEFINE_GDBUS_PROPERTY_INFOS (
			NM_DEFINE_DBUS_PROPERTY_INFO_EXTENDED_READABLE_L ("Options", "a{sv}",  NM_DHCP4_CONFIG_OPTIONS),
		),
	),
	.legacy_property_changed = TRUE,
};

static void
nm_dhcp4_config_class_init (NMDhcp4ConfigClass *config_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (config_class);
	NMDBusObjectClass *dbus_object_class = NM_DBUS_OBJECT_CLASS (config_class);

	object_class->get_property = get_property;
	object_class->finalize = finalize;

	dbus_object_class->export_path = NM_DBUS_EXPORT_PATH_NUMBERED (NM_DBUS_PATH"/DHCP4Config");
	dbus_object_class->interface_infos = NM_DBUS_INTERFACE_INFOS (&interface_info_dhcp4_config);
	dbus_object_class->export_on_construction = TRUE;

	obj_properties[PROP_OPTIONS] =
	     g_param_spec_variant (NM_DHCP4_CONFIG_OPTIONS, "", "",
	                           G_VARIANT_TYPE ("a{sv}"),
	                           NULL,
	                           G_PARAM_READABLE |
	                           G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties (object_class, _PROPERTY_ENUMS_LAST, obj_properties);
}
