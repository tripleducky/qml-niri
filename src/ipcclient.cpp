#include "ipcclient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcessEnvironment>
#include <QDebug>

IPCClient::IPCClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
{
    QObject::connect(m_socket, &QLocalSocket::readyRead,
                     this, &IPCClient::onReadyRead);
    QObject::connect(m_socket, &QLocalSocket::errorOccurred,
                     this, &IPCClient::onSocketError);
    QObject::connect(m_socket, &QLocalSocket::disconnected,
                     this, &IPCClient::disconnected);
}

IPCClient::~IPCClient()
{
    if (m_socket->isOpen()) {
        m_socket->close();
    }
}

bool IPCClient::connect()
{
    QString socketPath = QProcessEnvironment::systemEnvironment().value("NIRI_SOCKET");

    if (socketPath.isEmpty()) {
        emit errorOccurred("NIRI_SOCKET environment variable not set");
        return false;
    }

    qDebug() << "Connecting to niri socket:" << socketPath;
    m_socket->connectToServer(socketPath);

    if (!m_socket->waitForConnected(1000)) {
        emit errorOccurred("Failed to connect: " + m_socket->errorString());
        return false;
    }

    QByteArray data = "\"EventStream\"\n";
    qint64 written = m_socket->write(data);
    if (written != data.size()) {
        emit errorOccurred("Failed to write event stream request");
        return false;
    }
    m_socket->flush();

    emit connected();
    return true;
}

bool IPCClient::isConnected() const
{
    return m_socket && m_socket->state() == QLocalSocket::ConnectedState;
}

bool IPCClient::sendRequest(const QJsonObject &request)
{
    QJsonDocument doc(request);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";

    qint64 written = m_socket->write(data);
    if (written != data.size()) {
        emit errorOccurred("Failed to write request");
        return false;
    }

    m_socket->flush();
    return true;
}

void IPCClient::onReadyRead()
{
    m_readBuffer.append(m_socket->readAll());

    // Process complete lines (events are newline-delimited)
    int newlinePos;
    while ((newlinePos = m_readBuffer.indexOf('\n')) != -1) {
        QByteArray line = m_readBuffer.left(newlinePos);
        m_readBuffer.remove(0, newlinePos + 1);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << parseError.errorString();
            continue;
        }

        if (doc.isObject()) {
            QJsonObject obj = doc.object();

            // First response is the Reply to EventStream request
            if (obj.contains("Ok") || obj.contains("Err")) {
                if (obj.contains("Err")) {
                    emit errorOccurred("Event stream request failed: " +
                                      obj["Err"].toString());
                }
                continue;
            }

            // Subsequent messages are Events
            emit eventReceived(obj);
        }
    }
}

void IPCClient::onSocketError()
{
    emit errorOccurred(m_socket->errorString());
}
