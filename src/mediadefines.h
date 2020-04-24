#ifndef MEDIADEFINES_H
#define MEDIADEFINES_H

#include <QString>

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
    QString name;
    QString address;
    bool connected = false;
};

#endif // MEDIADEFINES_H
