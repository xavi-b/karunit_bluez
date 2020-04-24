#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

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
        this->address = device->address();
        this->name = device->name();
        this->connected = device->isConnected();
    }
};

struct BluezQtMediaTrack : public MediaTrack
{
    BluezQtMediaTrack(BluezQt::MediaPlayerTrack const& other)
    {
        album = other.album();
        artist = other.artist();
        duration = other.duration();
        genre = other.genre();
        numberOfTracks = other.numberOfTracks();
        title = other.title();
        trackNumber = other.trackNumber();
    }
};

class BluetoothManager : public KU::PLUGIN::PluginConnector
{
    Q_OBJECT
private:
    BluezQt::Manager* manager = nullptr;
    BluezQt::DevicePtr device = nullptr;

    void startAdapter(BluezQt::AdapterPtr adapter);
    void setupDevice(BluezQt::DevicePtr device);
    void connectDevice(BluezQt::DevicePtr device);
    void connectMediaPlayer(BluezQt::MediaPlayerPtr mediaPlayer);

public:
    BluetoothManager(QObject* parent = nullptr);
    void setup();

    void connectToDevice(DeviceInfo const& info);
    void disconnectFromDevice(DeviceInfo const& info);

    virtual void pluginSlot(QString const& signal, QVariantMap const& data) override;

    void mediaPreviousSlot();
    void mediaNextSlot();
    void mediaPlaySlot();
    void mediaPauseSlot();

    void emitKnownDevices(QList<DeviceInfo> const& devices);
    void emitDeviceConnected(DeviceInfo const& info);
    void emitDeviceDisconnected(DeviceInfo const& info);
    void emitTrackChanged(MediaTrack const& track);
    void emitNameChanged(QString const& name);
    void emitPositionChanged(quint32 position);
    void emitRepeatChanged(MediaRepeat repeat);
    void emitShuffleChanged(MediaShuffle shuffle);
    void emitStatusChanged(MediaStatus status);

signals:
    void knownDevices(QList<DeviceInfo> const& devices);
    void deviceConnected(DeviceInfo const& info);
    void deviceDisconnected(DeviceInfo const& info);
};

#endif // BLUETOOTHMANAGER_H
