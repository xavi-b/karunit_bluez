#ifndef BLUEZPLUGIN_H
#define BLUEZPLUGIN_H

#include <QtPlugin>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QDateTime>
#include "plugininterface.h"
#include "settings.h"
#include "settingswidget.h"
#include "bluetoothmanager.h"

class KU_Bluez_Plugin : public QObject, public KU::PLUGIN::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "xavi-b.karunit.PluginInterface")
    Q_INTERFACES(KU::PLUGIN::PluginInterface)

public:
    virtual ~KU_Bluez_Plugin();
    virtual QString name() const override;
    virtual QString id() const override;
    virtual KU::PLUGIN::PluginVersion version() const override;
    virtual QString license() const override;
    virtual QIcon icon() const override;
    virtual bool initialize() override;
    virtual bool stop() override;

    virtual QWidget* createWidget() override;
    virtual QWidget* createSettingsWidget() override;
    virtual QWidget* createAboutWidget() override;
    virtual bool loadSettings() override;
    virtual bool saveSettings() const override;

private:
    SettingsWidget* settingsWidget = nullptr;
    BluetoothManager* bluetoothManager= nullptr;
};


#endif // BLUEZPLUGIN_H
