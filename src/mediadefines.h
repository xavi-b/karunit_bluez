#ifndef MEDIADEFINES_H
#define MEDIADEFINES_H

#include <QString>
#include <QObject>

struct MediaTrack
{
    QString album;
    QString artist;
    quint32 duration;
    QString genre;
    quint32 numberOfTracks;
    QString title;
    quint32 trackNumber;
};

enum MediaRepeat
{
    RepeatOff,
    RepeatSingleTrack,
    RepeatAllTracks,
    RepeatGroup
};

enum MediaShuffle
{
    ShuffleOff,
    ShuffleAllTracks,
    ShuffleGroup
};

enum MediaStatus
{
    Playing,
    Stopped,
    Paused,
    ForwardSeek,
    ReverseSeek,
    Error
};

struct DeviceInfo
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString address MEMBER address CONSTANT)
    Q_PROPERTY(bool connected MEMBER connected CONSTANT)

public:
    QString name;
    QString address;
    bool    connected = false;

    bool operator==(DeviceInfo const& other) const
    {
        return name == other.name && address == other.address && connected == other.connected;
    }
};

#endif // MEDIADEFINES_H
