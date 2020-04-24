#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDebug>
#include "mediadefines.h"

class SettingsWidget : public QWidget
{
    Q_OBJECT
private:
    QListWidget* devicesListWidget;

public:
    SettingsWidget(QWidget* parent = nullptr);

    void deviceConnected(DeviceInfo const& info);
    void deviceDisconnected(DeviceInfo const& info);

    void setDevices(QList<DeviceInfo> const& list);

signals:
    void connectToDevice(DeviceInfo const& info);
    void disconnectFromDevice(DeviceInfo const& info);
    void log(QString const& log);
};

#endif // SETTINGSWIDGET_H
