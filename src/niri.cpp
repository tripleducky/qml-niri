#include "niri.h"
#include <QDebug>

Niri::Niri(QObject *parent)
    : QObject(parent)
    , m_ipcClient(new IPCClient(this))
{
    // Wire up IPC client signals
    QObject::connect(m_ipcClient, &IPCClient::connected,
                     this, &Niri::connected);
    QObject::connect(m_ipcClient, &IPCClient::disconnected,
                     this, &Niri::disconnected);
    QObject::connect(m_ipcClient, &IPCClient::errorOccurred,
                     this, &Niri::errorOccurred);
    QObject::connect(m_ipcClient, &IPCClient::eventReceived,
                     this, &Niri::rawEventReceived);
}

Niri::~Niri()
{
}

bool Niri::connect()
{
    return m_ipcClient->connect();
}

bool Niri::isConnected() const
{
    return m_ipcClient->isConnected();
}
