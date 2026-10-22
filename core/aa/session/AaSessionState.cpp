#include "AaSessionState.h"

QString AaSessionState::phaseName(Phase phase)
{
    switch (phase) {
    case Phase::Idle: return QStringLiteral("idle");
    case Phase::Accessory: return QStringLiteral("accessory");
    case Phase::Handshake: return QStringLiteral("handshake");
    case Phase::Discovery: return QStringLiteral("discovery");
    case Phase::Streaming: return QStringLiteral("streaming");
    case Phase::Closed: return QStringLiteral("closed");
    }
    return QStringLiteral("idle");
}

QJsonObject AaSessionState::toJson() const
{
    QJsonObject resolution;
    resolution.insert(QStringLiteral("w"), m_resolution.width());
    resolution.insert(QStringLiteral("h"), m_resolution.height());

    QJsonObject session;
    session.insert(QStringLiteral("id"), static_cast<qint64>(m_sessionId));
    session.insert(QStringLiteral("phase"), phaseName(m_phase));
    session.insert(QStringLiteral("phone"), m_phone);
    session.insert(QStringLiteral("codec"), m_codec);
    session.insert(QStringLiteral("resolution"), resolution);
    return session;
}
