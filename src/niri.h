#pragma once

#include <QObject>
#include "ipcclient.h"
#include "workspacemodel.h"

class Niri : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WorkspaceModel* workspaces READ workspaces CONSTANT)

public:
    explicit Niri(QObject *parent = nullptr);
    ~Niri();

    WorkspaceModel* workspaces() const { return m_workspaceModel; }

    Q_INVOKABLE bool connect();
    Q_INVOKABLE bool isConnected() const;

    Q_INVOKABLE void focusWorkspace(int index);
    Q_INVOKABLE void focusWorkspaceById(quint64 id);
    Q_INVOKABLE void focusWorkspaceByName(const QString &name);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);
    void rawEventReceived(const QJsonObject &event);

private:
    void sendAction(const QJsonObject &action);

    IPCClient *m_ipcClient = nullptr;
    WorkspaceModel *m_workspaceModel = nullptr;
};
