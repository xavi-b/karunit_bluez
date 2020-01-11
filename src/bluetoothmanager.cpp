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
            emit debugLog("Error initializing manager: " + job->errorText());
            return;
        }

        auto call = job->manager()->startService();
        call->waitForFinished();

        if(call->value() != 1 && call->value() != 2)
        {
            emit debugLog("Error starting service: " + QString::number(call->value().toUInt()));
            return;
        }

        connect(job->manager(), &BluezQt::Manager::usableAdapterChanged, this, [=](BluezQt::AdapterPtr adapter)
        {
            emit debugLog("usableAdapterChanged: " + adapter->name());
            startAdapter(adapter);
        });

        emit debugLog("Adapters count: " + QString::number(job->manager()->adapters().size()));
        for(auto& adapter : job->manager()->adapters())
        {
            emit debugLog("Adapter found: " + adapter->name());
            startAdapter(adapter);
        }

        if (!job->manager()->usableAdapter())
        {
            emit debugLog("No usable adapter");
            return;
        }

        QList<KU::PLUGIN::DeviceInfo> list;
        for(auto& d : this->manager->devices())
            list.append(BluezQtHostInfo(d.data()));
        emit knownDevices(list);

        NoInputNoOutputAgent* agent = new NoInputNoOutputAgent();
        connect(agent, &NoInputNoOutputAgent::serviceAuthorized, this, [=](BluezQt::DevicePtr device, const QString &uuid, bool allowed)
        {
            emit debugLog((allowed ? "Accepted" : "Rejected") + QString(" service: " + uuid + " from " + device->friendlyName()));
        });

        job->manager()->registerAgent(agent);
        job->manager()->requestDefaultAgent(agent);
    });
}

void BluetoothManager::startAdapter(BluezQt::AdapterPtr adapter)
{
    adapter->setPowered(true)->waitForFinished();
    adapter->setName("BluezQt")->waitForFinished();
    adapter->setPairable(true)->waitForFinished();
    adapter->setDiscoverable(true)->waitForFinished();

    for(auto& device : adapter->devices())
    {
        emit debugLog("Device found: " + device->friendlyName() + " " + (device->isConnected() ? "Connected" : "Disconnected"));

        if(device->isConnected())
        {
            connectDevice(device);
            emit deviceConnected(BluezQtHostInfo(device.data()));
        }

        connect(device.data(), &BluezQt::Device::connectedChanged, this, [=](bool connected)
        {
            BluezQt::Device* device = qobject_cast<BluezQt::Device*>(sender());
            if(device != nullptr)
            {
                emit debugLog("Device connected: " + device->friendlyName() + " " + (connected ? "Connected" : "Disconnected"));
                if(connected)
                {
                    connectDevice(BluezQt::DevicePtr(device));
                    emit deviceConnected(BluezQtHostInfo(device));
                }
                else
                {
                    if(this->device->address() == device->address())
                        this->device = nullptr;
                    emit deviceDisconnected(BluezQtHostInfo(device));
                }
            }
            else
            {
                emit debugLog("Device connected: UNKNOWN");
                this->device = nullptr;
            }
        });
    }
}

void BluetoothManager::connectDevice(BluezQt::DevicePtr device)
{
    this->device = device;
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
        emit trackChanged(BluezQtMediaTrack(mediaPlayer->track()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::trackChanged, this, [=](BluezQt::MediaPlayerTrack track)
        {
            emit trackChanged(BluezQtMediaTrack(track));
        });

        emit nameChanged(mediaPlayer->name());
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::nameChanged, this, [=](QString const& name)
        {
            emit nameChanged(name);
        });

        emit positionChanged(mediaPlayer->position());
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::positionChanged, this, [=](quint32 position)
        {
            emit positionChanged(position);
        });

        emit repeatChanged(static_cast<KU::PLUGIN::MediaRepeat>(mediaPlayer->repeat()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::repeatChanged, this, [=](BluezQt::MediaPlayer::Repeat repeat)
        {
            emit repeatChanged(static_cast<KU::PLUGIN::MediaRepeat>(repeat));
        });

        emit shuffleChanged(static_cast<KU::PLUGIN::MediaShuffle>(mediaPlayer->shuffle()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::shuffleChanged, this, [=](BluezQt::MediaPlayer::Shuffle shuffle)
        {
            emit shuffleChanged(static_cast<KU::PLUGIN::MediaShuffle>(shuffle));
        });

        emit statusChanged(static_cast<KU::PLUGIN::MediaStatus>(mediaPlayer->status()));
        connect(mediaPlayer.data(), &BluezQt::MediaPlayer::statusChanged, this, [=](BluezQt::MediaPlayer::Status status)
        {
            emit statusChanged(static_cast<KU::PLUGIN::MediaStatus>(status));
        });
    }
}

BluetoothManager::BluetoothManager(QObject* parent)
    : KU::PLUGIN::BluetoothConnector(parent)
{

}

void BluetoothManager::mediaPrevious()
{
    if(this->device != nullptr)
        this->device->mediaPlayer()->previous();
}

void BluetoothManager::mediaNext()
{
    if(this->device != nullptr)
        this->device->mediaPlayer()->next();
}

void BluetoothManager::mediaPlay()
{
    if(this->device != nullptr)
        this->device->mediaPlayer()->play();
}

void BluetoothManager::mediaPause()
{
    if(this->device != nullptr)
        this->device->mediaPlayer()->pause();
}

void BluetoothManager::connectToDevice(KU::PLUGIN::DeviceInfo const& info)
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

void BluetoothManager::disconnectFromDevice(const KU::PLUGIN::DeviceInfo& info)
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
