#include "niri.h"
#include <QDebug>

Niri::Niri(QObject *parent)
    : QObject(parent)
    , m_ipcClient(new IPCClient(this))
    , m_workspaceModel(new WorkspaceModel(this))
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

    // Wire events to workspace model
    QObject::connect(m_ipcClient, &IPCClient::eventReceived,
                     m_workspaceModel, &WorkspaceModel::handleEvent);
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
