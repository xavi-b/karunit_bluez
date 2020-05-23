#include "plugin.h"

KU_Bluez_Plugin::~KU_Bluez_Plugin()
{
    if(this->bluetoothManager != nullptr)
        delete this->bluetoothManager;
}

QString KU_Bluez_Plugin::name() const
{
    return "Bluez";
}

QString KU_Bluez_Plugin::id() const
{
    return "bluez";
}

KU::PLUGIN::PluginVersion KU_Bluez_Plugin::version() const
{
    return { 1, 0, 0 };
}

QString KU_Bluez_Plugin::license() const
{
    return "LGPL";
}

QIcon KU_Bluez_Plugin::icon() const
{
    return QIcon();
}

bool KU_Bluez_Plugin::initialize()
{
    this->bluetoothManager = new BluetoothManager;
    this->setPluginConnector(this->bluetoothManager);

    this->settingsWidget = new SettingsWidget;
    QObject::connect(this->settingsWidget, &SettingsWidget::log,
                     this->getPluginConnector(), &KU::PLUGIN::PluginConnector::log);

    QObject::connect(this->bluetoothManager, &BluetoothManager::knownDevices,
                     this->settingsWidget, &SettingsWidget::setDevices);
    QObject::connect(this->bluetoothManager, &BluetoothManager::deviceConnected,
                     this->settingsWidget, &SettingsWidget::deviceConnected);
    QObject::connect(this->bluetoothManager, &BluetoothManager::deviceDisconnected,
                     this->settingsWidget, &SettingsWidget::deviceDisconnected);
    QObject::connect(this->settingsWidget, &SettingsWidget::connectToDevice,
                     this->bluetoothManager, &BluetoothManager::connectToDevice);
    QObject::connect(this->settingsWidget, &SettingsWidget::disconnectFromDevice,
                     this->bluetoothManager, &BluetoothManager::disconnectFromDevice);

    this->bluetoothManager->setup();

    return true;
}

bool KU_Bluez_Plugin::stop()
{
    return true;
}

QWidget* KU_Bluez_Plugin::createWidget()
{
    return nullptr;
}

QWidget* KU_Bluez_Plugin::createSettingsWidget()
{
    return this->settingsWidget;
}

QWidget* KU_Bluez_Plugin::createAboutWidget()
{
    return nullptr;
}

bool KU_Bluez_Plugin::loadSettings()
{
    return true;
}

bool KU_Bluez_Plugin::saveSettings() const
{
    return KU::Settings::instance()->status() == QSettings::NoError;
}
