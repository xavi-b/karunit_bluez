#ifndef BLUEZPLUGINCONNECTOR_H
#define BLUEZPLUGINCONNECTOR_H

#include <QVariant>
#include <BluezQt/Manager>
#include <BluezQt/InitManagerJob>
#include <BluezQt/Adapter>
#include <BluezQt/Agent>
#include <BluezQt/Device>
#include <BluezQt/MediaPlayer>
#include <BluezQt/PendingCall>
#include "plugininterface.h"
#include "noinputnooutputagent.h"
#include "mediadefines.h"

class BluezQtHostInfo : public DeviceInfo
{
public:
    BluezQtHostInfo(BluezQt::Device const* device)
        : DeviceInfo()
    {
        this->address   = device->address();
        this->name      = device->name();
        this->connected = device->isConnected();
    }
};

struct BluezQtMediaTrack : public MediaTrack
{
    BluezQtMediaTrack(BluezQt::MediaPlayerTrack const& other)
    {
        album          = other.album();
        artist         = other.artist();
        duration       = other.duration();
        genre          = other.genre();
        numberOfTracks = other.numberOfTracks();
        title          = other.title();
        trackNumber    = other.trackNumber();
    }
};

class KU_Bluez_PluginConnector : public KU::PLUGIN::PluginConnector
{
    Q_OBJECT

    Q_PROPERTY(QVariantList devices READ variantDevices NOTIFY devicesChanged)

public:
    KU_Bluez_PluginConnector(QObject* parent = nullptr);
    void setup();

    Q_INVOKABLE void scan();
    Q_INVOKABLE void connectToDevice(DeviceInfo const& info);
    Q_INVOKABLE void disconnectFromDevice(DeviceInfo const& info);

    virtual void pluginSlot(QString const& signal, QVariantMap const& data) override;

    void mediaPreviousSlot();
    void mediaNextSlot();
    void mediaPlaySlot();
    void mediaPauseSlot();

    void emitDeviceConnected(DeviceInfo const& info);
    void emitDeviceDisconnected(DeviceInfo const& info);
    void emitTrackChanged(MediaTrack const& track);
    void emitNameChanged(QString const& name);
    void emitPositionChanged(quint32 position);
    void emitRepeatChanged(MediaRepeat repeat);
    void emitShuffleChanged(MediaShuffle shuffle);
    void emitStatusChanged(MediaStatus status);

    QVariantList variantDevices();

signals:
    void devicesChanged();

private:
    QList<DeviceInfo>   devices;
    BluezQt::Manager*   manager         = nullptr;
    BluezQt::AdapterPtr adapter         = nullptr;
    BluezQt::Device*    connectedDevice = nullptr;

    void startAdapter(BluezQt::AdapterPtr adapter);
    void setupDevice(BluezQt::DevicePtr device);
    void connectDevice(BluezQt::Device* device);
    void disconnectDevice(BluezQt::Device* device);
    void connectMediaPlayer(BluezQt::MediaPlayerPtr mediaPlayer);

    void logDeviceInfos(BluezQt::Device* device) const;
};

#endif // BLUEZPLUGINCONNECTOR_H
