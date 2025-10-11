#pragma once

#include <QObject>
#include "ipcclient.h"

class Niri : public QObject
{
    Q_OBJECT

public:
    explicit Niri(QObject *parent = nullptr);
    ~Niri();

    Q_INVOKABLE bool connect();
    Q_INVOKABLE bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);
    void rawEventReceived(const QJsonObject &event);

private:
    IPCClient *m_ipcClient = nullptr;
};
