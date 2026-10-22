#include "AoaSwitcher.h"
#include "AoaUsbPort.h"
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTest>

class FakeUsbPort final : public AoaUsbPort
{
public:
    bool start(QString *) override { return true; }
    void requestAccessoryMode(quint64 id) override { requestedIds.append(id); }
    bool openAccessory(quint64, QString *) override { return true; }
    bool resetDevice(quint64 id, QString *) override
    {
        resetIds.append(id);
        return true;
    }

    QList<quint64> requestedIds;
    QList<quint64> resetIds;
};

static UsbDeviceInfo deviceFrom(const QJsonObject &object)
{
    UsbDeviceInfo info;
    info.id = static_cast<quint64>(object.value(QStringLiteral("id")).toInt());
    info.vendorId = object.value(QStringLiteral("vendorId")).toString().toUShort(nullptr, 16);
    info.productId = object.value(QStringLiteral("productId")).toString().toUShort(nullptr, 16);
    info.deviceClass = static_cast<quint8>(object.value(QStringLiteral("deviceClass")).toInt());
    const QJsonArray classes = object.value(QStringLiteral("interfaceClasses")).toArray();
    for (const QJsonValue &value : classes) {
        info.interfaceClasses.append(static_cast<quint8>(value.toInt()));
    }
    info.serial = object.value(QStringLiteral("serial")).toString();
    return info;
}

static QList<quint64> idsFrom(const QJsonArray &array)
{
    QList<quint64> ids;
    for (const QJsonValue &value : array) {
        ids.append(static_cast<quint64>(value.toInt()));
    }
    return ids;
}

// Replays a trace-derived scenario against a fake port. An unmatched ignoreMessage fails the
// test function, which proves each marker line was emitted.
static void runFixture(const QString &fileName)
{
    // Any aoap: or reset: warning fails the scenario
    QTest::failOnWarning(QRegularExpression(QStringLiteral(".*")));

    QFile file(QStringLiteral(AOA_FIXTURES_DIR) + QLatin1Char('/') + fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    QVERIFY(document.isObject());
    const QJsonObject root = document.object();

    QHash<QString, UsbDeviceInfo> devices;
    const QJsonObject deviceObjects = root.value(QStringLiteral("devices")).toObject();
    for (auto it = deviceObjects.begin(); it != deviceObjects.end(); ++it) {
        devices.insert(it.key(), deviceFrom(it.value().toObject()));
    }

    FakeUsbPort port;
    AoaSwitcher switcher(&port);

    const QJsonArray steps = root.value(QStringLiteral("steps")).toArray();
    for (const QJsonValue &stepValue : steps) {
        const QJsonObject step = stepValue.toObject();
        if (step.contains(QStringLiteral("log"))) {
            QTest::ignoreMessage(QtInfoMsg, QRegularExpression(step.value(QStringLiteral("log")).toString()));
        }

        const QString event = step.value(QStringLiteral("event")).toString();
        const UsbDeviceInfo device = devices.value(step.value(QStringLiteral("device")).toString());
        if (event == QLatin1String("start")) {
            QVERIFY(switcher.start());
        } else if (event == QLatin1String("arrive")) {
            emit port.deviceArrived(device, step.value(QStringLiteral("atStartup")).toBool());
        } else if (event == QLatin1String("requested")) {
            emit port.accessoryModeRequested(device.id);
        } else if (event == QLatin1String("leave")) {
            emit port.deviceLeft(device.id);
        } else {
            QFAIL(qPrintable(QStringLiteral("unknown event: %1").arg(event)));
        }

        QCOMPARE(AoaSwitcher::stateName(switcher.state()), step.value(QStringLiteral("state")).toString());
    }

    QCOMPARE(port.requestedIds, idsFrom(root.value(QStringLiteral("requests")).toArray()));
    QCOMPARE(port.resetIds, idsFrom(root.value(QStringLiteral("resets")).toArray()));
}

class AoaSwitcherReplayTest : public QObject
{
    Q_OBJECT

private slots:
    void successScenario();
    void leftoverResetScenario();
};

void AoaSwitcherReplayTest::successScenario()
{
    runFixture(QStringLiteral("success.json"));
}

void AoaSwitcherReplayTest::leftoverResetScenario()
{
    runFixture(QStringLiteral("leftover-reset.json"));
}

QTEST_GUILESS_MAIN(AoaSwitcherReplayTest)
#include "AoaSwitcherReplayTest.moc"
