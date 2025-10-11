#include <QQmlExtensionPlugin>
#include <QQmlEngine>
#include "niriclient.h"

class NiriPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(uri == QLatin1String("Niri"));
        qmlRegisterType<NiriClient>(uri, 0, 1, "NiriClient");
    }
};

#include "plugin.moc"
