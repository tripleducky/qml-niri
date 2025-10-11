#include "niri.h"
#include <QDebug>
#include <QJsonObject>

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

void Niri::focusWorkspace(int index)
{
    QJsonObject reference;
    reference["Index"] = index;

    QJsonObject action;
    action["FocusWorkspace"] = QJsonObject{{"reference", reference}};

    sendAction(action);
}

void Niri::focusWorkspaceById(quint64 id)
{
    QJsonObject reference;
    reference["Id"] = QJsonValue::fromVariant(id);

    QJsonObject action;
    action["FocusWorkspace"] = QJsonObject{{"reference", reference}};

    sendAction(action);
}

void Niri::focusWorkspaceByName(const QString &name)
{
    QJsonObject reference;
    reference["Name"] = name;

    QJsonObject action;
    action["FocusWorkspace"] = QJsonObject{{"reference", reference}};

    sendAction(action);
}

void Niri::sendAction(const QJsonObject &action)
{
    if (!isConnected()) {
        qWarning() << "Cannot send action: not connected to niri";
        return;
    }

    QJsonObject request;
    request["Action"] = action;

    m_ipcClient->sendRequest(request);
}
