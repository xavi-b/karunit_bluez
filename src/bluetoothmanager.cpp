#include "bluetoothmanager.h"

void BluetoothManager::setup()
{
    this->manager = new BluezQt::Manager();
    BluezQt::InitManagerJob* job = this->manager->init();
    job->start();
    connect(job, &BluezQt::InitManagerJob::result, this, [=](BluezQt::InitManagerJob* job)
    {
        if (job->error())
        {
            emitLogSignal("Error initializing manager: " + job->errorText());
            return;
        }

        auto call = job->manager()->startService();
        call->waitForFinished();

        if(call->value() != 1 && call->value() != 2)
        {
            emitLogSignal("Error starting service: " + QString::number(call->value().toUInt()));
            return;
        }

        connect(job->manager(), &BluezQt::Manager::usableAdapterChanged, this, [=](BluezQt::AdapterPtr adapter)
        {
            emitLogSignal("usableAdapterChanged: " + adapter->name());
            startAdapter(adapter);
        });

        NoInputNoOutputAgent* agent = new NoInputNoOutputAgent();
        connect(agent, &NoInputNoOutputAgent::serviceAuthorized, this, [=](BluezQt::DevicePtr device, const QString &uuid, bool allowed)
        {
            emitLogSignal((allowed ? "Accepted" : "Rejected") + QString(" service: " + uuid + " from " + device->friendlyName()));
        });
        job->manager()->registerAgent(agent)->waitForFinished();
        job->manager()->requestDefaultAgent(agent)->waitForFinished();

        emitLogSignal("Adapters count: " + QString::number(job->manager()->adapters().size()));
        for(auto& adapter : job->manager()->adapters())
        {
            emitLogSignal("Adapter found: " + adapter->name());
            startAdapter(adapter);
        }

        if (!job->manager()->usableAdapter())
        {
            emitLogSignal(XB::Log("No usable adapter"));
            return;
        }

        QList<DeviceInfo> list;
        for(auto& d : this->manager->devices())
            list.append(BluezQtHostInfo(d.data()));
        emitKnownDevices(list);
    });
}

void BluetoothManager::startAdapter(BluezQt::AdapterPtr adapter)
{
    adapter->setPowered(true)->waitForFinished();
    adapter->setName("karunit")->waitForFinished();
    adapter->setPairable(true)->waitForFinished();
    adapter->setDiscoverable(true)->waitForFinished();

    for(auto& device : adapter->devices())
    {
        emitLogSignal("Device found: " + device->friendlyName() + " " + (device->isConnected() ? "Connected" : "Disconnected"));
        this->setupDevice(device);
    }

    connect(adapter.data(), &BluezQt::Adapter::deviceAdded, this, [=](BluezQt::DevicePtr device)
    {
        emitLogSignal("Device added: " + device->friendlyName() + " " + (device->isConnected() ? "Connected" : "Disconnected"));
        this->setupDevice(device);
    });
}

void BluetoothManager::setupDevice(BluezQt::DevicePtr device)
{
    if(device->isConnected())
    {
        connectDevice(device);
        emitDeviceConnected(BluezQtHostInfo(device.data()));
    }

    connect(device.data(), &BluezQt::Device::connectedChanged, this, [=](bool connected)
    {
        BluezQt::Device* device = qobject_cast<BluezQt::Device*>(sender());
        if(device != nullptr)
        {
            emitLogSignal("Device connected: " + device->friendlyName() + " " + (connected ? "Connected" : "Disconnected"));
            if(connected)
            {
                connectDevice(BluezQt::DevicePtr(device));
                emitDeviceConnected(BluezQtHostInfo(device));
            }
            else
            {
                if(this->device->address() == device->address())
                    this->device = nullptr;
                emitDeviceDisconnected(BluezQtHostInfo(device));
            }
        }
        else
        {
            emitLogSignal(XB::Log("Device connected: UNKNOWN"));
            this->device = nullptr;
        }
    });

    connect(device.data(), &BluezQt::Device::deviceChanged, this, [=](BluezQt::DevicePtr device)
    {
        if(device != nullptr)
        {
            emitLogSignal("Device changed: " + device->friendlyName() + " " + device->address());
            emitLogSignal("Address:" + device->address());
            emitLogSignal("Class:" + QString::number(device->deviceClass()));
            emitLogSignal("Icon:" + device->icon());
            emitLogSignal("Legacy Pairing:" + QString(device->hasLegacyPairing() ? "yes" : "no"));
            emitLogSignal("Name:" + device->name());
            emitLogSignal("Paired:" + QString(device->isPaired() ? "yes" : "no"));
            emitLogSignal("Trusted:" + QString(device->isTrusted() ? "yes" : "no"));
            emitLogSignal("Services:" + device->uuids().join(" + "));
        }
    });
}

void BluetoothManager::connectDevice(BluezQt::DevicePtr device)
{
    this->device = device;
    this->device->setTrusted(true)->waitForFinished();
    connectMediaPlayer(this->device->mediaPlayer());
    connect(this->device.data(), &BluezQt::Device::mediaPlayerChanged, this, [=](BluezQt::MediaPlayerPtr mediaPlayer)
    {
        connectMediaPlayer(mediaPlayer);
    });
}

void BluetoothManager::connectMediaPlayer(BluezQt::MediaPlayerPtr mediaPlayer)
{
    if(mediaPlayer != nullptr)
    {
        emitTrackChanged(BluezQtMediaTrack(mediaPlayer->track()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::trackChanged, this, [=](BluezQt::MediaPlayerTrack track)
        {
            emitTrackChanged(BluezQtMediaTrack(track));
        });

        emitNameChanged(mediaPlayer->name());
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::nameChanged, this, [=](QString const& name)
        {
            emitNameChanged(name);
        });

        emitPositionChanged(mediaPlayer->position());
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::positionChanged, this, [=](quint32 position)
        {
            emitPositionChanged(position);
        });

        emitRepeatChanged(static_cast<MediaRepeat>(mediaPlayer->repeat()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::repeatChanged, this, [=](BluezQt::MediaPlayer::Repeat repeat)
        {
            emitRepeatChanged(static_cast<MediaRepeat>(repeat));
        });

        emitShuffleChanged(static_cast<MediaShuffle>(mediaPlayer->shuffle()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::shuffleChanged, this, [=](BluezQt::MediaPlayer::Shuffle shuffle)
        {
            emitShuffleChanged(static_cast<MediaShuffle>(shuffle));
        });

        emitStatusChanged(static_cast<MediaStatus>(mediaPlayer->status()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::statusChanged, this, [=](BluezQt::MediaPlayer::Status status)
        {
            emitStatusChanged(static_cast<MediaStatus>(status));
        });
    }
}

BluetoothManager::BluetoothManager(QObject* parent)
    : KU::PLUGIN::PluginConnector(parent)
{

}


void BluetoothManager::connectToDevice(DeviceInfo const& info)
{
    for(auto& d : this->manager->usableAdapter()->devices())
    {
        if(d->address() == info.address)
        {
            d->connectToDevice();
            break;
        }
    }
}

void BluetoothManager::disconnectFromDevice(DeviceInfo const& info)
{
    for(auto& d : this->manager->usableAdapter()->devices())
    {
        if(d->address() == info.address)
        {
            d->disconnectFromDevice();
            break;
        }
    }
}

void BluetoothManager::pluginSlot(QString const& signal, QVariantMap const& data)
{
    if(signal == "mediaPrevious")
    {
        this->mediaPreviousSlot();
        return;
    }

    if(signal == "mediaNext")
    {
        this->mediaNextSlot();
        return;
    }

    if(signal == "mediaPlay")
    {
        this->mediaPlaySlot();
        return;
    }

    if(signal == "mediaPause")
    {
        this->mediaPauseSlot();
        return;
    }
}

void BluetoothManager::mediaPreviousSlot()
{
    if(this->device == nullptr)
    {
        emitLogSignal(XB::Log("mediaPreviousSlot() No device"));
        return;
    }

    if(this->device->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaPreviousSlot() No media player"));
        return;
    }

    this->device->mediaPlayer()->previous();
}

void BluetoothManager::mediaNextSlot()
{
    if(this->device == nullptr)
    {
        emitLogSignal(XB::Log("mediaNextSlot() No device"));
        return;
    }

    if(this->device->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaNextSlot() No media player"));
        return;
    }

    this->device->mediaPlayer()->next();
}

void BluetoothManager::mediaPlaySlot()
{
    if(this->device == nullptr)
    {
        emitLogSignal(XB::Log("mediaPlaySlot() No device"));
        return;
    }

    if(this->device->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaPlaySlot() No media player"));
        return;
    }

    this->device->mediaPlayer()->play();
}

void BluetoothManager::mediaPauseSlot()
{
    if(this->device == nullptr)
    {
        emitLogSignal(XB::Log("mediaPauseSlot() No device"));
        return;
    }

    if(this->device->mediaPlayer() == nullptr)
    {
        emitLogSignal(XB::Log("mediaPauseSlot() No media player"));
        return;
    }

    this->device->mediaPlayer()->pause();
}

void BluetoothManager::emitKnownDevices(const QList<DeviceInfo>& devices)
{
    emit knownDevices(devices);
}

void BluetoothManager::emitDeviceConnected(const DeviceInfo& info)
{
    emit deviceConnected(info);
}

void BluetoothManager::emitDeviceDisconnected(const DeviceInfo& info)
{
    emit deviceDisconnected(info);
}

void BluetoothManager::emitTrackChanged(const MediaTrack& track)
{

}

void BluetoothManager::emitNameChanged(const QString& name)
{

}

void BluetoothManager::emitPositionChanged(quint32 position)
{

}

void BluetoothManager::emitRepeatChanged(MediaRepeat repeat)
{

}

void BluetoothManager::emitShuffleChanged(MediaShuffle shuffle)
{

}

void BluetoothManager::emitStatusChanged(MediaStatus status)
{

}
