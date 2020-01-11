#include "noinputnooutputagent.h"

NoInputNoOutputAgent::NoInputNoOutputAgent(QObject *parent)
    : BluezQt::Agent(parent)
{

}

QDBusObjectPath NoInputNoOutputAgent::objectPath() const
{
    return QDBusObjectPath("/Agent/NoInputNoOutput");
}

BluezQt::Agent::Capability NoInputNoOutputAgent::capability() const
{
    return Agent::NoInputNoOutput;
}

void NoInputNoOutputAgent::authorizeService(BluezQt::DevicePtr device, const QString &uuid, const BluezQt::Request<> &request)
{
    request.accept();
    emit serviceAuthorized(device, uuid, true);
}
