#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QSocketNotifier>
#include <QJsonDocument>

class NiriClient : public QObject
{
    Q_OBJECT

public:
    explicit NiriClient(QObject *parent = nullptr);
    ~NiriClient();

    Q_INVOKABLE bool connect();
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);
    void eventReceived(const QJsonObject &event);

private slots:
    void onReadyRead();
    void onSocketError();

private:
    bool sendRequest(const QJsonObject &request);

    QLocalSocket *m_socket = nullptr;
    QByteArray m_readBuffer;
};
