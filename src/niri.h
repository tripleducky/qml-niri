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

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);
    void rawEventReceived(const QJsonObject &event);

private:
    IPCClient *m_ipcClient = nullptr;
    WorkspaceModel *m_workspaceModel = nullptr;
};
