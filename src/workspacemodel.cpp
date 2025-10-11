#include <algorithm>
#include <QDebug>
#include <QJsonArray>
#include "workspacemodel.h"

WorkspaceModel::WorkspaceModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int WorkspaceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_workspaces.count();
}

QVariant WorkspaceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_workspaces.count())
        return QVariant();

    const Workspace &ws = m_workspaces.at(index.row());

    switch (role) {
    case IdRole:
        return QVariant::fromValue(ws.id);
    case IndexRole:
        return ws.index;
    case NameRole:
        return ws.name;
    case OutputRole:
        return ws.output;
    case IsActiveRole:
        return ws.isActive;
    case IsFocusedRole:
        return ws.isFocused;
    case IsUrgentRole:
        return ws.isUrgent;
    case ActiveWindowIdRole:
        return QVariant::fromValue(ws.activeWindowId);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WorkspaceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[IndexRole] = "index";
    roles[NameRole] = "name";
    roles[OutputRole] = "output";
    roles[IsActiveRole] = "isActive";
    roles[IsFocusedRole] = "isFocused";
    roles[IsUrgentRole] = "isUrgent";
    roles[ActiveWindowIdRole] = "activeWindowId";
    return roles;
}

void WorkspaceModel::handleEvent(const QJsonObject &event)
{
    if (event.contains("WorkspacesChanged")) {
        QJsonArray workspaces = event["WorkspacesChanged"].toObject()["workspaces"].toArray();
        handleWorkspacesChanged(workspaces);
    }
    else if (event.contains("WorkspaceActivated")) {
        QJsonObject data = event["WorkspaceActivated"].toObject();
        quint64 id = data["id"].toInteger();
        bool focused = data["focused"].toBool();
        handleWorkspaceActivated(id, focused);
    }
    else if (event.contains("WorkspaceUrgencyChanged")) {
        QJsonObject data = event["WorkspaceUrgencyChanged"].toObject();
        quint64 id = data["id"].toInteger();
        bool urgent = data["urgent"].toBool();
        handleWorkspaceUrgencyChanged(id, urgent);
    }
    else if (event.contains("WorkspaceActiveWindowChanged")) {
        QJsonObject data = event["WorkspaceActiveWindowChanged"].toObject();
        quint64 workspaceId = data["workspace_id"].toInteger();
        QJsonValue activeWindowId = data["active_window_id"];
        handleWorkspaceActiveWindowChanged(workspaceId, activeWindowId);
    }
}

void WorkspaceModel::handleWorkspacesChanged(const QJsonArray &workspaces)
{
    beginResetModel();
    m_workspaces.clear();

    for (const QJsonValue &value : workspaces) {
        if (value.isObject()) {
            m_workspaces.append(parseWorkspace(value.toObject()));
        }
    }

    // Sort by index (which corresponds to workspace position on its output)
    std::sort(m_workspaces.begin(), m_workspaces.end(),
              [](const Workspace &a, const Workspace &b) {
                  // First sort by output name, then by index within output
                  if (a.output != b.output) {
                      return a.output < b.output;
                  }
                  return a.index < b.index;
              });

    endResetModel();
    emit countChanged();
}

void WorkspaceModel::handleWorkspaceActivated(quint64 id, bool focused)
{
    int idx = findWorkspaceIndex(id);
    if (idx == -1) {
        qWarning() << "Activated workspace not found:" << id;
        return;
    }

    const QString &output = m_workspaces[idx].output;

    // Update all workspaces on the same output
    for (int i = 0; i < m_workspaces.count(); ++i) {
        if (m_workspaces[i].output == output) {
            bool becameActive = (i == idx);
            if (m_workspaces[i].isActive != becameActive) {
                m_workspaces[i].isActive = becameActive;
                QModelIndex modelIdx = index(i);
                emit dataChanged(modelIdx, modelIdx, {IsActiveRole});
            }
        }

        // If focused, update all workspaces' focus state
        if (focused) {
            bool becameFocused = (i == idx);
            if (m_workspaces[i].isFocused != becameFocused) {
                m_workspaces[i].isFocused = becameFocused;
                QModelIndex modelIdx = index(i);
                emit dataChanged(modelIdx, modelIdx, {IsFocusedRole});
            }
        }
    }
}

void WorkspaceModel::handleWorkspaceUrgencyChanged(quint64 id, bool urgent)
{
    int idx = findWorkspaceIndex(id);
    if (idx == -1) {
        qWarning() << "Workspace not found for urgency change:" << id;
        return;
    }

    if (m_workspaces[idx].isUrgent != urgent) {
        m_workspaces[idx].isUrgent = urgent;
        QModelIndex modelIdx = index(idx);
        emit dataChanged(modelIdx, modelIdx, {IsUrgentRole});
    }
}

void WorkspaceModel::handleWorkspaceActiveWindowChanged(quint64 workspaceId, const QJsonValue &activeWindowId)
{
    int idx = findWorkspaceIndex(workspaceId);
    if (idx == -1) {
        qWarning() << "Workspace not found for active window change:" << workspaceId;
        return;
    }

    quint64 newActiveWindowId = activeWindowId.isNull() ? 0 : activeWindowId.toInteger();
    if (m_workspaces[idx].activeWindowId != newActiveWindowId) {
        m_workspaces[idx].activeWindowId = newActiveWindowId;
        QModelIndex modelIdx = index(idx);
        emit dataChanged(modelIdx, modelIdx, {ActiveWindowIdRole});
    }
}

Workspace WorkspaceModel::parseWorkspace(const QJsonObject &obj)
{
    Workspace ws;
    ws.id = obj["id"].toInteger();
    ws.index = obj["idx"].toInt();
    ws.name = obj["name"].toString();
    ws.output = obj["output"].toString();
    ws.isActive = obj["is_active"].toBool();
    ws.isFocused = obj["is_focused"].toBool();
    ws.isUrgent = obj["is_urgent"].toBool();

    QJsonValue activeWindowId = obj["active_window_id"];
    ws.activeWindowId = activeWindowId.isNull() ? 0 : activeWindowId.toInteger();

    return ws;
}

int WorkspaceModel::findWorkspaceIndex(quint64 id) const
{
    for (int i = 0; i < m_workspaces.count(); ++i) {
        if (m_workspaces[i].id == id)
            return i;
    }
    return -1;
}
