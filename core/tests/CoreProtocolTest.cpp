#include "AaSessionState.h"
#include "CoreEndpointConfig.h"
#include "CoreServer.h"
#include "Protocol.h"
#include "VehicleModel.h"
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>
#include <QWebSocket>

static constexpr int WAIT_MS = 5000;
static const QByteArray KEYFRAME_FIXTURE = QByteArrayLiteral("KEYFRAME-FIXTURE");

static QJsonObject parseObject(const QString &text)
{
    return QJsonDocument::fromJson(text.toUtf8()).object();
}

// Waits for the next text message and returns its error code, or an empty string if none arrives
static QString nextErrorCode(QSignalSpy &textSpy)
{
    if (textSpy.isEmpty() && !textSpy.wait(WAIT_MS)) {
        return QString();
    }
    return parseObject(textSpy.takeFirst().at(0).toString()).value(QStringLiteral("code")).toString();
}

class CoreProtocolTest : public QObject
{
    Q_OBJECT

private slots:
    void helloSnapshotSurvivesReconnect();
    void wrongVersionIsRejected();
    void vehicleSetIsGuarded();
    void vehicleSetWritesExactValue();
    void invalidClientIdIsRejected();
};

void CoreProtocolTest::helloSnapshotSurvivesReconnect()
{
    VehicleModel vehicle;
    vehicle.setSpeed(42);
    vehicle.setDriverTemp(23.5);

    AaSessionState session;
    session.setSessionId(7);
    session.setPhase(AaSessionState::Phase::Streaming);
    session.setKeyframe(KEYFRAME_FIXTURE);

    CoreServer server(&vehicle, &session);
    QVERIFY(server.listen(0));
    QCOMPARE(server.address(), QHostAddress(QHostAddress::LocalHost));

    for (int round = 0; round < 2; ++round) {
        QWebSocket socket;
        QSignalSpy connectedSpy(&socket, &QWebSocket::connected);
        QSignalSpy textSpy(&socket, &QWebSocket::textMessageReceived);
        QSignalSpy binarySpy(&socket, &QWebSocket::binaryMessageReceived);
        QSignalSpy disconnectedSpy(&socket, &QWebSocket::disconnected);

        socket.open(CoreEndpointConfig::serverUrl(server.port()));
        QVERIFY(connectedSpy.wait(WAIT_MS));

        Protocol::HelloInfo hello;
        hello.clientId = QStringLiteral("test");
        hello.role = QString(Protocol::Role::DASH);
        hello.displayId = QStringLiteral("main");
        hello.width = 1920;
        hello.height = 1080;
        hello.wants = {QString(Protocol::Want::VIDEO), QString(Protocol::Want::VEHICLE)};
        socket.sendTextMessage(Protocol::toText(Protocol::makeHello(hello)));

        QVERIFY(textSpy.wait(WAIT_MS));
        const QJsonObject snapshot = parseObject(textSpy.first().at(0).toString());
        QCOMPARE(snapshot.value(QStringLiteral("type")).toString(), QStringLiteral("snapshot"));
        QCOMPARE(snapshot.value(QStringLiteral("v")).toInt(), 1);
        const QJsonObject vehicleJson = snapshot.value(QStringLiteral("vehicle")).toObject();
        QCOMPARE(vehicleJson.value(QStringLiteral("speed")).toInt(), 42);
        QCOMPARE(vehicleJson.value(QStringLiteral("driverTemp")).toDouble(), 23.5);
        const QJsonObject sessionJson = snapshot.value(QStringLiteral("session")).toObject();
        QCOMPARE(sessionJson.value(QStringLiteral("id")).toInt(), 7);
        QCOMPARE(sessionJson.value(QStringLiteral("phase")).toString(), QStringLiteral("streaming"));

        if (binarySpy.isEmpty()) {
            QVERIFY(binarySpy.wait(WAIT_MS));
        }
        QCOMPARE(binarySpy.first().at(0).toByteArray(), KEYFRAME_FIXTURE);

        socket.close();
        QVERIFY(disconnectedSpy.wait(WAIT_MS));
        QTRY_COMPARE(server.clientCount(), 0);
        QCOMPARE(session.phase(), AaSessionState::Phase::Streaming);
        QCOMPARE(session.sessionId(), 7u);
    }
}

void CoreProtocolTest::wrongVersionIsRejected()
{
    VehicleModel vehicle;
    AaSessionState session;
    CoreServer server(&vehicle, &session);
    QVERIFY(server.listen(0));

    QWebSocket socket;
    QSignalSpy connectedSpy(&socket, &QWebSocket::connected);
    QSignalSpy textSpy(&socket, &QWebSocket::textMessageReceived);

    socket.open(CoreEndpointConfig::serverUrl(server.port()));
    QVERIFY(connectedSpy.wait(WAIT_MS));

    socket.sendTextMessage(QStringLiteral(R"({"type":"hello","v":2,"client_id":"test","role":"dash"})"));

    QVERIFY(textSpy.wait(WAIT_MS));
    const QJsonObject error = parseObject(textSpy.takeFirst().at(0).toString());
    QCOMPARE(error.value(QStringLiteral("type")).toString(), QStringLiteral("error"));
    QCOMPARE(error.value(QStringLiteral("v")).toInt(), 1);
    QCOMPARE(error.value(QStringLiteral("code")).toString(), QStringLiteral("unsupported_version"));

    QVERIFY(!textSpy.wait(300));
}

void CoreProtocolTest::vehicleSetIsGuarded()
{
    VehicleModel vehicle;
    QSignalSpy publishSpy(&vehicle, &VehicleModel::publishRequested);
    AaSessionState session;
    CoreServer server(&vehicle, &session);
    QVERIFY(server.listen(0));

    QWebSocket socket;
    QSignalSpy connectedSpy(&socket, &QWebSocket::connected);
    QSignalSpy textSpy(&socket, &QWebSocket::textMessageReceived);

    socket.open(CoreEndpointConfig::serverUrl(server.port()));
    QVERIFY(connectedSpy.wait(WAIT_MS));

    // Before hello
    socket.sendTextMessage(Protocol::toText(Protocol::makeVehicleSet(QStringLiteral("Climate"), 0, QJsonObject())));
    QCOMPARE(nextErrorCode(textSpy), QStringLiteral("not_greeted"));

    Protocol::HelloInfo hello;
    hello.clientId = QStringLiteral("test");
    hello.role = QString(Protocol::Role::DASH);
    socket.sendTextMessage(Protocol::toText(Protocol::makeHello(hello)));
    QVERIFY(textSpy.wait(WAIT_MS));
    QCOMPARE(parseObject(textSpy.takeFirst().at(0).toString()).value(QStringLiteral("type")).toString(),
             QStringLiteral("snapshot"));

    // Group outside the client allow-list
    socket.sendTextMessage(Protocol::toText(Protocol::makeVehicleSet(QStringLiteral("Engine"), 0, QJsonObject())));
    QCOMPARE(nextErrorCode(textSpy), QStringLiteral("forbidden_group"));
    QCOMPARE(publishSpy.count(), 0);

    // values is not an object
    socket.sendTextMessage(QStringLiteral(R"({"type":"vehicle_set","v":1,"group":"Climate","values":5})"));
    QCOMPARE(nextErrorCode(textSpy), QStringLiteral("invalid_message"));
    QCOMPARE(publishSpy.count(), 0);
}

void CoreProtocolTest::vehicleSetWritesExactValue()
{
    VehicleModel vehicle;
    vehicle.setDriverTemp(23.5);
    QSignalSpy publishSpy(&vehicle, &VehicleModel::publishRequested);
    AaSessionState session;
    CoreServer server(&vehicle, &session);
    QVERIFY(server.listen(0));

    QWebSocket socket;
    QSignalSpy connectedSpy(&socket, &QWebSocket::connected);
    QSignalSpy textSpy(&socket, &QWebSocket::textMessageReceived);

    socket.open(CoreEndpointConfig::serverUrl(server.port()));
    QVERIFY(connectedSpy.wait(WAIT_MS));

    Protocol::HelloInfo hello;
    hello.clientId = QStringLiteral("test");
    hello.role = QString(Protocol::Role::CLIMATE);
    socket.sendTextMessage(Protocol::toText(Protocol::makeHello(hello)));
    QVERIFY(textSpy.wait(WAIT_MS));
    textSpy.clear();

    QJsonObject values;
    values.insert(QStringLiteral("driverTemp"), 23.52);
    socket.sendTextMessage(Protocol::toText(Protocol::makeVehicleSet(QStringLiteral("Climate"), 0, values)));

    QTRY_COMPARE(publishSpy.count(), 1);
    QCOMPARE(publishSpy.first().at(0).value<VehicleModel::Group>(), VehicleModel::Group::Climate);
    QCOMPARE(publishSpy.first().at(1).value<quint8>(), quint8(0));
    QCOMPARE(vehicle.driverTemp(), 23.52);
}

void CoreProtocolTest::invalidClientIdIsRejected()
{
    VehicleModel vehicle;
    AaSessionState session;
    CoreServer server(&vehicle, &session);
    QVERIFY(server.listen(0));

    QWebSocket socket;
    QSignalSpy connectedSpy(&socket, &QWebSocket::connected);
    QSignalSpy textSpy(&socket, &QWebSocket::textMessageReceived);
    QSignalSpy disconnectedSpy(&socket, &QWebSocket::disconnected);

    socket.open(CoreEndpointConfig::serverUrl(server.port()));
    QVERIFY(connectedSpy.wait(WAIT_MS));

    Protocol::HelloInfo hello;
    hello.role = QString(Protocol::Role::DASH);

    hello.clientId = QStringLiteral("bad\nid");
    socket.sendTextMessage(Protocol::toText(Protocol::makeHello(hello)));
    QCOMPARE(nextErrorCode(textSpy), QStringLiteral("invalid_hello"));

    hello.clientId = QString(65, QLatin1Char('a'));
    socket.sendTextMessage(Protocol::toText(Protocol::makeHello(hello)));
    QCOMPARE(nextErrorCode(textSpy), QStringLiteral("invalid_hello"));

    QVERIFY(socket.state() == QAbstractSocket::ConnectedState);
    QVERIFY(disconnectedSpy.isEmpty());
}

QTEST_GUILESS_MAIN(CoreProtocolTest)
#include "CoreProtocolTest.moc"
