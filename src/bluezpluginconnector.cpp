#include "bluezpluginconnector.h"

void KU_Bluez_PluginConnector::setup()
{
    this->manager                = new BluezQt::Manager();
    BluezQt::InitManagerJob* job = this->manager->init();
    job->start();
    connect(job, &BluezQt::InitManagerJob::result, this, [=](BluezQt::InitManagerJob* job) {
        if (job->error())
        {
            emitLogSignal("Error initializing manager: " + job->errorText());
            return;
        }

        auto call = job->manager()->startService();
        call->waitForFinished();

        if (call->value() != 1 && call->value() != 2)
        {
            emitLogSignal("Error starting service: " + QString::number(call->value().toUInt()));
            return;
        }

        connect(job->manager(), &BluezQt::Manager::usableAdapterChanged, this, [=](BluezQt::AdapterPtr adapter) {
            emitLogSignal("usableAdapterChanged: " + adapter->name());
            startAdapter(adapter);
        });

        NoInputNoOutputAgent* agent = new NoInputNoOutputAgent();
        connect(agent,
                &NoInputNoOutputAgent::serviceAuthorized,
                this,
                [=](BluezQt::DevicePtr device, const QString& uuid, bool allowed) {
                    emitLogSignal((allowed ? "Accepted" : "Rejected") +
                                  QString(" service: " + uuid + " from " + device->friendlyName()));
                });
        job->manager()->registerAgent(agent)->waitForFinished();
        job->manager()->requestDefaultAgent(agent)->waitForFinished();

        emitLogSignal("Adapters count: " + QString::number(job->manager()->adapters().size()));
        for (auto& adapter : job->manager()->adapters())
        {
            emitLogSignal("Adapter found: " + adapter->name());
            startAdapter(adapter);
        }

        if (!job->manager()->usableAdapter())
        {
            emitLogSignal(XB::Log("No usable adapter"));
            return;
        }

        //        devices.clear();
        //        for (auto& d : this->manager->devices())
        //            devices.append(BluezQtHostInfo(d.data()));
        //        emit devicesChanged();
    });
}

void KU_Bluez_PluginConnector::scan()
{
    if (this->adapter)
        this->adapter->startDiscovery()->waitForFinished();
}

void KU_Bluez_PluginConnector::startAdapter(BluezQt::AdapterPtr adapter)
{
    this->adapter = adapter;
    adapter->setPowered(true)->waitForFinished();
    adapter->setName("karunit")->waitForFinished();
    adapter->setPairable(true)->waitForFinished();
    adapter->setDiscoverable(true)->waitForFinished();
    adapter->setDiscoverableTimeout(0)->waitForFinished();
    adapter->startDiscovery()->waitForFinished();

    emitLogSignal("UUIDS: " + adapter->uuids().join(" + "));

    for (auto& device : adapter->devices())
    {
        emitLogSignal("Device found: " + device->friendlyName() + " " +
                      (device->isConnected() ? "Connected" : "Disconnected"));
        this->setupDevice(device);
    }

    connect(adapter.data(), &BluezQt::Adapter::deviceAdded, this, [=](BluezQt::DevicePtr device) {
        emitLogSignal("Device added: " + device->friendlyName() + " " +
                      (device->isConnected() ? "Connected" : "Disconnected"));
        this->setupDevice(device);
    });
}

void KU_Bluez_PluginConnector::setupDevice(BluezQt::DevicePtr device)
{
    devices.append(BluezQtHostInfo(device.data()));
    emit devicesChanged();

    if (device->isConnected())
    {
        connectDevice(device.get());
        emitDeviceConnected(BluezQtHostInfo(device.data()));
    }

    connect(device.data(), &BluezQt::Device::connectedChanged, this, [=](bool connected) {
        BluezQt::Device* device = qobject_cast<BluezQt::Device*>(sender());
        if (device != nullptr)
        {
            emitLogSignal("Device connected: " + device->friendlyName() + " " +
                          (connected ? "Connected" : "Disconnected"));
            this->logDeviceInfos(device);
            if (connected)
            {
                connectDevice(device);
                emitDeviceConnected(BluezQtHostInfo(device));
            }
            else
            {
                disconnectDevice(device);
                emitDeviceDisconnected(BluezQtHostInfo(device));
            }
        }
        else
        {
            emitLogSignal(XB::Log("Device connected: UNKNOWN"));
        }
    });

    connect(device.data(), &BluezQt::Device::deviceChanged, this, [=](BluezQt::DevicePtr device) {
        if (device != nullptr)
        {
            this->logDeviceInfos(device.get());
        }
    });
}

void KU_Bluez_PluginConnector::connectDevice(BluezQt::Device* device)
{
    this->connectedDevice = device;
    device->setTrusted(true)->waitForFinished();
    connectMediaPlayer(device->mediaPlayer());
    connect(device, &BluezQt::Device::mediaPlayerChanged, this, [=](BluezQt::MediaPlayerPtr mediaPlayer) {
        connectMediaPlayer(mediaPlayer);
    });
}

void KU_Bluez_PluginConnector::disconnectDevice(BluezQt::Device* device)
{
    this->connectedDevice = nullptr;
    this->adapter->removeDevice(device->toSharedPtr());
}

void KU_Bluez_PluginConnector::connectMediaPlayer(BluezQt::MediaPlayerPtr mediaPlayer)
{
    if (mediaPlayer != nullptr)
    {
        emitTrackChanged(BluezQtMediaTrack(mediaPlayer->track()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::trackChanged, this, [=](BluezQt::MediaPlayerTrack track) {
            emitTrackChanged(BluezQtMediaTrack(track));
        });

        emitNameChanged(mediaPlayer->name());
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::nameChanged, this, [=](QString const& name) {
            emitNameChanged(name);
        });

        emitPositionChanged(mediaPlayer->position());
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::positionChanged, this, [=](quint32 position) {
            emitPositionChanged(position);
        });

        emitRepeatChanged(static_cast<MediaRepeat>(mediaPlayer->repeat()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::repeatChanged, this, [=](BluezQt::MediaPlayer::Repeat repeat) {
            emitRepeatChanged(static_cast<MediaRepeat>(repeat));
        });

        emitShuffleChanged(static_cast<MediaShuffle>(mediaPlayer->shuffle()));
        connect(mediaPlayer.data(),
                &BluezQt::MediaPlayer::shuffleChanged,
                this,
                [=](BluezQt::MediaPlayer::Shuffle shuffle) {
                    emitShuffleChanged(static_cast<MediaShuffle>(shuffle));
                });

        emitStatusChanged(static_cast<MediaStatus>(mediaPlayer->status()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::statusChanged, this, [=](BluezQt::MediaPlayer::Status status) {
            emitStatusChanged(static_cast<MediaStatus>(status));
        });
    }
}

void KU_Bluez_PluginConnector::logDeviceInfos(BluezQt::Device* device) const
{
    emitLogSignal("Device changed: " + device->friendlyName() + " " + device->address());
    emitLogSignal("Address: " + device->address());
    emitLogSignal("Class: " + QString::number(device->deviceClass()));
    emitLogSignal("Icon: " + device->icon());
    emitLogSignal("Legacy Pairing: " + QString(device->hasLegacyPairing() ? "yes" : "no"));
    emitLogSignal("Name: " + device->name());
    emitLogSignal("Paired: " + QString(device->isPaired() ? "yes" : "no"));
    emitLogSignal("Trusted: " + QString(device->isTrusted() ? "yes" : "no"));
    emitLogSignal("Services: " + device->uuids().join(" + "));
}

KU_Bluez_PluginConnector::KU_Bluez_PluginConnector(QObject* parent)
    : KU::PLUGIN::PluginConnector(parent)
{
}

void KU_Bluez_PluginConnector::connectToDevice(DeviceInfo const& info)
{
    for (auto& d : this->manager->usableAdapter()->devices())
    {
        if (d->address() == info.address)
        {
            d->connectToDevice();
            break;
        }
    }
}

void KU_Bluez_PluginConnector::disconnectFromDevice(DeviceInfo const& info)
{
    for (auto& d : this->manager->usableAdapter()->devices())
    {
        if (d->address() == info.address)
        {
            d->disconnectFromDevice();
            break;
        }
    }
}

void KU_Bluez_PluginConnector::pluginSlot(QString const& signal, QVariantMap const& data)
{
    if (signal == "mediaPrevious")
    {
        this->mediaPreviousSlot();
        return;
    }

    if (signal == "mediaNext")
    {
        this->mediaNextSlot();
        return;
    }

    if (signal == "mediaPlay")
    {
        this->mediaPlaySlot();
        return;
    }

    if (signal == "mediaPause")
    {
        this->mediaPauseSlot();
        return;
    }
}

void KU_Bluez_PluginConnector::mediaPreviousSlot()
{
    if (this->connectedDevice == nullptr)
    {
        emitLogSignal(XB::Log("mediaPreviousSlot() No device"));
        return;
    }

    if (this->connectedDevice->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaPreviousSlot() No media player"));
        return;
    }

    this->connectedDevice->mediaPlayer()->previous();
}

void KU_Bluez_PluginConnector::mediaNextSlot()
{
    if (this->connectedDevice == nullptr)
    {
        emitLogSignal(XB::Log("mediaNextSlot() No device"));
        return;
    }

    if (this->connectedDevice->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaNextSlot() No media player"));
        return;
    }

    this->connectedDevice->mediaPlayer()->next();
}

void KU_Bluez_PluginConnector::mediaPlaySlot()
{
    if (this->connectedDevice == nullptr)
    {
        emitLogSignal(XB::Log("mediaPlaySlot() No device"));
        return;
    }

    if (this->connectedDevice->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaPlaySlot() No media player"));
        return;
    }

    this->connectedDevice->mediaPlayer()->play();
}

void KU_Bluez_PluginConnector::mediaPauseSlot()
{
    if (this->connectedDevice == nullptr)
    {
        emitLogSignal(XB::Log("mediaPauseSlot() No device"));
        return;
    }

    if (this->connectedDevice->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaPauseSlot() No media player"));
        return;
    }

    this->connectedDevice->mediaPlayer()->pause();
}

void KU_Bluez_PluginConnector::emitDeviceConnected(const DeviceInfo& info)
{
    bool found = false;
    for (auto& d : this->devices)
    {
        if (d.name == info.name)
        {
            d.connected = true;
            found       = true;
            break;
        }
    }

    if (!found)
        devices.append(info);

    emit devicesChanged();
}

void KU_Bluez_PluginConnector::emitDeviceDisconnected(const DeviceInfo& info)
{
    bool found = false;
    for (auto& d : this->devices)
    {
        if (d.name == info.name)
        {
            d.connected = false;
            found       = true;
            break;
        }
    }

    if (!found)
        devices.append(info);

    emit devicesChanged();
}

void KU_Bluez_PluginConnector::emitTrackChanged(const MediaTrack& track)
{
    QVariantMap data;
    data["title"]    = track.title;
    data["artist"]   = track.artist;
    data["duration"] = track.duration;
    this->pluginDataSignal("trackChanged", data);
}

void KU_Bluez_PluginConnector::emitNameChanged(const QString& name)
{
}

void KU_Bluez_PluginConnector::emitPositionChanged(quint32 position)
{
    QVariantMap data;
    data["position"] = position;
    this->pluginDataSignal("positionChanged", data);
}

void KU_Bluez_PluginConnector::emitRepeatChanged(MediaRepeat repeat)
{
}

void KU_Bluez_PluginConnector::emitShuffleChanged(MediaShuffle shuffle)
{
}

void KU_Bluez_PluginConnector::emitStatusChanged(MediaStatus status)
{
    QVariantMap data;
    data["status"] = (int)status;
    this->pluginDataSignal("statusChanged", data);
}

QVariantList KU_Bluez_PluginConnector::variantDevices()
{
    QVariantList list;

    for (const auto& l : devices)
        list.append(QVariant::fromValue(l));

    return list;
}
