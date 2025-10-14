#pragma once

#include <QAbstractListModel>
#include <QJsonObject>

struct Window {
    quint64 id;
    QString title;
    QString appId;
    qint32 pid;
    quint64 workspaceId;
    bool isFocused;
    bool isFloating;
    bool isUrgent;
    QString iconPath;
};

class WindowModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(Window* focusedWindow READ focusedWindow NOTIFY focusedWindowChanged)

public:
    enum WindowRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        AppIdRole,
        PidRole,
        WorkspaceIdRole,
        IsFocusedRole,
        IsFloatingRole,
        IsUrgentRole,
        IconPathRole
    };

    explicit WindowModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Window* focusedWindow() const { return m_focusedWindow; }

public slots:
    void handleEvent(const QJsonObject &event);

signals:
    void countChanged();
    void focusedWindowChanged();

private:
    void handleWindowsChanged(const QJsonArray &windows);
    void handleWindowOpenedOrChanged(const QJsonObject &window);
    void handleWindowClosed(quint64 id);
    void handleWindowFocusChanged(const QJsonValue &idValue);
    void handleWindowUrgencyChanged(quint64 id, bool urgent);
    void handleWindowLayoutsChanged(const QJsonArray &changes);

    Window parseWindow(const QJsonObject &obj);
    int findWindowIndex(quint64 id) const;
    void updateFocusedWindow();

    QList<Window> m_windows;
    Window *m_focusedWindow = nullptr;
};
