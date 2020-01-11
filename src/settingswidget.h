#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDebug>
#include "bluetoothinterface.h"

class SettingsWidget : public QWidget
{
    Q_OBJECT
private:
    QListWidget* devicesListWidget;

public:
    SettingsWidget(QWidget* parent = nullptr);

    void deviceConnected(KU::PLUGIN::DeviceInfo const& info);
    void deviceDisconnected(KU::PLUGIN::DeviceInfo const& info);

    void setDevices(QList<KU::PLUGIN::DeviceInfo> const& list);

signals:
    void connectToDevice(KU::PLUGIN::DeviceInfo const& info);
    void disconnectFromDevice(KU::PLUGIN::DeviceInfo const& info);
    void log(QString const& log);
};

#endif // SETTINGSWIDGET_H
