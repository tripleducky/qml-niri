#include <algorithm>
#include <QDebug>
#include <QJsonArray>
#include "windowmodel.h"

WindowModel::WindowModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int WindowModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_windows.count();
}

QVariant WindowModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_windows.count())
        return QVariant();

    const Window &win = m_windows.at(index.row());

    switch (role) {
    case IdRole:
        return QVariant::fromValue(win.id);
    case TitleRole:
        return win.title;
    case AppIdRole:
        return win.appId;
    case PidRole:
        return win.pid;
    case WorkspaceIdRole:
        return QVariant::fromValue(win.workspaceId);
    case IsFocusedRole:
        return win.isFocused;
    case IsFloatingRole:
        return win.isFloating;
    case IsUrgentRole:
        return win.isUrgent;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WindowModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[AppIdRole] = "appId";
    roles[PidRole] = "pid";
    roles[WorkspaceIdRole] = "workspaceId";
    roles[IsFocusedRole] = "isFocused";
    roles[IsFloatingRole] = "isFloating";
    roles[IsUrgentRole] = "isUrgent";
    return roles;
}

void WindowModel::handleEvent(const QJsonObject &event)
{
    if (event.contains("WindowsChanged")) {
        QJsonArray windows = event["WindowsChanged"].toObject()["windows"].toArray();
        handleWindowsChanged(windows);
    }
    else if (event.contains("WindowOpenedOrChanged")) {
        QJsonObject window = event["WindowOpenedOrChanged"].toObject()["window"].toObject();
        handleWindowOpenedOrChanged(window);
    }
    else if (event.contains("WindowClosed")) {
        quint64 id = event["WindowClosed"].toObject()["id"].toInteger();
        handleWindowClosed(id);
    }
    else if (event.contains("WindowFocusChanged")) {
        QJsonValue id = event["WindowFocusChanged"].toObject()["id"];
        handleWindowFocusChanged(id);
    }
    else if (event.contains("WindowUrgencyChanged")) {
        QJsonObject data = event["WindowUrgencyChanged"].toObject();
        quint64 id = data["id"].toInteger();
        bool urgent = data["urgent"].toBool();
        handleWindowUrgencyChanged(id, urgent);
    }
    else if (event.contains("WindowLayoutsChanged")) {
        QJsonArray changes = event["WindowLayoutsChanged"].toObject()["changes"].toArray();
        handleWindowLayoutsChanged(changes);
    }
}

void WindowModel::handleWindowsChanged(const QJsonArray &windows)
{
    beginResetModel();
    m_windows.clear();

    for (const QJsonValue &value : windows) {
        if (value.isObject()) {
            m_windows.append(parseWindow(value.toObject()));
        }
    }

    endResetModel();
    emit countChanged();
    updateFocusedWindow();
}

void WindowModel::handleWindowOpenedOrChanged(const QJsonObject &windowObj)
{
    Window window = parseWindow(windowObj);
    int idx = findWindowIndex(window.id);

    if (idx == -1) {
        // New window
        beginInsertRows(QModelIndex(), m_windows.count(), m_windows.count());
        m_windows.append(window);
        endInsertRows();
        emit countChanged();
    } else {
        // Existing window changed
        m_windows[idx] = window;
        QModelIndex modelIdx = index(idx);
        emit dataChanged(modelIdx, modelIdx);
    }

    // If this window is focused, update all other windows
    if (window.isFocused) {
        for (int i = 0; i < m_windows.count(); ++i) {
            if (m_windows[i].id != window.id && m_windows[i].isFocused) {
                m_windows[i].isFocused = false;
                QModelIndex modelIdx = index(i);
                emit dataChanged(modelIdx, modelIdx, {IsFocusedRole});
            }
        }
        updateFocusedWindow();
    }
}

void WindowModel::handleWindowClosed(quint64 id)
{
    int idx = findWindowIndex(id);
    if (idx == -1) {
        qWarning() << "Window not found for close:" << id;
        return;
    }

    bool wasFocused = m_windows[idx].isFocused;

    beginRemoveRows(QModelIndex(), idx, idx);
    m_windows.removeAt(idx);
    endRemoveRows();

    emit countChanged();

    if (wasFocused) {
        updateFocusedWindow();
    }
}

void WindowModel::handleWindowFocusChanged(const QJsonValue &idValue)
{
    quint64 newFocusedId = idValue.isNull() ? 0 : idValue.toInteger();

    for (int i = 0; i < m_windows.count(); ++i) {
        bool shouldBeFocused = (m_windows[i].id == newFocusedId);
        if (m_windows[i].isFocused != shouldBeFocused) {
            m_windows[i].isFocused = shouldBeFocused;
            QModelIndex modelIdx = index(i);
            emit dataChanged(modelIdx, modelIdx, {IsFocusedRole});
        }
    }

    updateFocusedWindow();
}

void WindowModel::handleWindowUrgencyChanged(quint64 id, bool urgent)
{
    int idx = findWindowIndex(id);
    if (idx == -1) {
        qWarning() << "Window not found for urgency change:" << id;
        return;
    }

    if (m_windows[idx].isUrgent != urgent) {
        m_windows[idx].isUrgent = urgent;
        QModelIndex modelIdx = index(idx);
        emit dataChanged(modelIdx, modelIdx, {IsUrgentRole});
    }
}

void WindowModel::handleWindowLayoutsChanged(const QJsonArray &changes)
{
    // Window layout changes don't affect the properties we're tracking
    // This is mostly for position/size which we're not exposing yet
    Q_UNUSED(changes);
}

Window WindowModel::parseWindow(const QJsonObject &obj)
{
    Window win;
    win.id = obj["id"].toInteger();
    win.title = obj["title"].toString();
    win.appId = obj["app_id"].toString();

    QJsonValue pidValue = obj["pid"];
    win.pid = pidValue.isNull() ? -1 : pidValue.toInt();

    QJsonValue workspaceIdValue = obj["workspace_id"];
    win.workspaceId = workspaceIdValue.isNull() ? 0 : workspaceIdValue.toInteger();

    win.isFocused = obj["is_focused"].toBool();
    win.isFloating = obj["is_floating"].toBool();
    win.isUrgent = obj["is_urgent"].toBool();

    return win;
}

int WindowModel::findWindowIndex(quint64 id) const
{
    for (int i = 0; i < m_windows.count(); ++i) {
        if (m_windows[i].id == id)
            return i;
    }
    return -1;
}

void WindowModel::updateFocusedWindow()
{
    Window *newFocused = nullptr;

    for (Window &win : m_windows) {
        if (win.isFocused) {
            newFocused = &win;
            break;
        }
    }

    if (m_focusedWindow != newFocused) {
        m_focusedWindow = newFocused;
        emit focusedWindowChanged();
    }
}
