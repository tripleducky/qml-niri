#include "ipcclient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcessEnvironment>
#include <QDebug>

IPCClient::IPCClient(QObject *parent)
    : QObject(parent)
    , m_eventSocket(new QLocalSocket(this))
    , m_requestSocket(new QLocalSocket(this))
{
    QObject::connect(m_eventSocket, &QLocalSocket::readyRead,
                     this, &IPCClient::onReadyRead);
    QObject::connect(m_eventSocket, &QLocalSocket::errorOccurred,
                     this, &IPCClient::onSocketError);
    QObject::connect(m_eventSocket, &QLocalSocket::disconnected,
                     this, &IPCClient::disconnected);
}

IPCClient::~IPCClient()
{
    if (m_eventSocket->isOpen()) {
        m_eventSocket->close();
    }
    if (m_requestSocket->isOpen()) {
        m_requestSocket->close();
    }
}

bool IPCClient::connect()
{
    m_socketPath = QProcessEnvironment::systemEnvironment().value("NIRI_SOCKET");

    if (m_socketPath.isEmpty()) {
        emit errorOccurred("NIRI_SOCKET environment variable not set");
        return false;
    }

    qDebug() << "Connecting to niri socket for events:" << m_socketPath;
    m_eventSocket->connectToServer(m_socketPath);

    if (!m_eventSocket->waitForConnected(1000)) {
        emit errorOccurred("Failed to connect event socket: " + m_eventSocket->errorString());
        return false;
    }

    qDebug() << "Connecting to niri socket for requests:" << m_socketPath;
    m_requestSocket->connectToServer(m_socketPath);

    if (!m_requestSocket->waitForConnected(1000)) {
        emit errorOccurred("Failed to connect request socket: " + m_requestSocket->errorString());
        m_eventSocket->close();
        return false;
    }

    qDebug() << "Listening to niri event stream ...";
    QByteArray data = "\"EventStream\"\n";
    qint64 written = m_eventSocket->write(data);
    if (written != data.size()) {
        emit errorOccurred("Failed to write event stream request");
        return false;
    }
    m_eventSocket->flush();

    emit connected();
    return true;
}

bool IPCClient::isConnected() const
{
    return m_eventSocket && m_eventSocket->state() == QLocalSocket::ConnectedState &&
           m_requestSocket && m_requestSocket->state() == QLocalSocket::ConnectedState;
}

bool IPCClient::sendRequest(const QJsonObject &request)
{
    if (!m_requestSocket || m_requestSocket->state() != QLocalSocket::ConnectedState) {
        qWarning() << "Request socket not connected";
        return false;
    }

    QJsonDocument doc(request);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";

    qDebug() << "Sending request:" << data;

    qint64 written = m_requestSocket->write(data);
    if (written != data.size()) {
        emit errorOccurred("Failed to write request");
        return false;
    }

    m_requestSocket->flush();

    // Wait for and read the response (blocking, but should be fast)
    if (m_requestSocket->waitForReadyRead(1000)) {
        QByteArray response = m_requestSocket->readLine();
        qDebug() << "Response:" << response;

        QJsonParseError parseError;
        QJsonDocument responseDoc = QJsonDocument::fromJson(response, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse response:" << parseError.errorString();
            return false;
        }

        QJsonObject responseObj = responseDoc.object();
        if (responseObj.contains("Err")) {
            qWarning() << "Request error:" << responseObj["Err"].toString();
            return false;
        }
    }

    return true;
}

void IPCClient::onReadyRead()
{
    m_readBuffer.append(m_eventSocket->readAll());

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
    emit errorOccurred(m_eventSocket->errorString());
}
