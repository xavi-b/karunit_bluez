#ifndef NOINPUTNOOUTPUTAGENT_H
#define NOINPUTNOOUTPUTAGENT_H

#include <BluezQt/Agent>
#include <QDBusObjectPath>

class NoInputNoOutputAgent : public BluezQt::Agent
{
    Q_OBJECT
public:
    NoInputNoOutputAgent(QObject* parent = nullptr);
    QDBusObjectPath objectPath() const override;
    BluezQt::Agent::Capability capability() const override;
    void authorizeService(BluezQt::DevicePtr device, const QString &uuid, const BluezQt::Request<> &request) override;

signals:
    void serviceAuthorized(BluezQt::DevicePtr device, const QString &uuid, bool allowed);
};

#endif // NOINPUTNOOUTPUTAGENT_H
