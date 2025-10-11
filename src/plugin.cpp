#include <QQmlExtensionPlugin>
#include <QQmlEngine>
#include "niri.h"

class NiriPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(uri == QLatin1String("Niri"));
        qmlRegisterType<Niri>(uri, 0, 1, "Niri");
    }
};

#include "plugin.moc"
