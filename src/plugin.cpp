#include "plugin.h"
#include <QQmlEngine>

QString KU_Bluez_Plugin::name() const
{
    return "Bluez";
}

QString KU_Bluez_Plugin::id() const
{
    return "karunit_bluez";
}

KU::PLUGIN::PluginVersion KU_Bluez_Plugin::version() const
{
    return {1, 0, 0};
}

QString KU_Bluez_Plugin::license() const
{
    return "LGPL";
}

QString KU_Bluez_Plugin::icon() const
{
    return ":/karunit_bluez/res/bluetooth.png";
}

bool KU_Bluez_Plugin::initialize()
{
    qmlRegisterSingletonInstance("KarunitPlugins", 1, 0, "KUPBluezPluginConnector", this->pluginConnector);

    this->getPluginConnector()->setup();

    return true;
}

bool KU_Bluez_Plugin::stop()
{
    return true;
}

bool KU_Bluez_Plugin::loadSettings()
{
    return true;
}

bool KU_Bluez_Plugin::saveSettings() const
{
    return KU::Settings::instance()->status() == QSettings::NoError;
}

KU_Bluez_PluginConnector* KU_Bluez_Plugin::getPluginConnector()
{
    if (this->pluginConnector == nullptr)
        this->pluginConnector = new KU_Bluez_PluginConnector;
    return qobject_cast<KU_Bluez_PluginConnector*>(this->pluginConnector);
}
