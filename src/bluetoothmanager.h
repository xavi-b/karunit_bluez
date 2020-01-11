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
#include "bluetoothinterface.h"
#include "noinputnooutputagent.h"

class BluezQtHostInfo : public KU::PLUGIN::DeviceInfo
{
public:
    BluezQtHostInfo(BluezQt::Device const* device)
        : KU::PLUGIN::DeviceInfo()
    {
        this->address = device->address();
        this->name = device->name();
        this->connected = device->isConnected();
    }
};

struct BluezQtMediaTrack : public KU::PLUGIN::MediaTrack
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

class BluetoothManager : public KU::PLUGIN::BluetoothConnector
{
    Q_OBJECT
private:
    BluezQt::Manager* manager = nullptr;
    BluezQt::DevicePtr device = nullptr;

    void startAdapter(BluezQt::AdapterPtr adapter);
    void connectDevice(BluezQt::DevicePtr device);
    void connectMediaPlayer(BluezQt::MediaPlayerPtr mediaPlayer);

public:
    BluetoothManager(QObject* parent = nullptr);
    void setup();

    virtual void mediaPrevious() override;
    virtual void mediaNext() override;
    virtual void mediaPlay() override;
    virtual void mediaPause() override;
    virtual void connectToDevice(KU::PLUGIN::DeviceInfo const& info) override;
    virtual void disconnectFromDevice(KU::PLUGIN::DeviceInfo const& info) override;

signals:
    void debugLog(QString const& log);
};

#endif // BLUETOOTHMANAGER_H
