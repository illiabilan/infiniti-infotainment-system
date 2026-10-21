#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QSize>
#include <QString>
#include <QtGlobal>

// Data-only boundary of the Android Auto session. The Z tickets will drive it.
// It holds only what the Core snapshot reports and has no CAN dependency.
class AaSessionState
{
public:
    enum class Phase { Idle, Accessory, Handshake, Discovery, Streaming, Closed };

    static QString phaseName(Phase phase);

    quint32 sessionId() const { return m_sessionId; }
    void setSessionId(quint32 id) { m_sessionId = id; }

    Phase phase() const { return m_phase; }
    void setPhase(Phase phase) { m_phase = phase; }

    void setPhone(const QString &phone) { m_phone = phone; }
    void setCodec(const QString &codec) { m_codec = codec; }
    void setResolution(QSize resolution) { m_resolution = resolution; }

    QByteArray keyframe() const { return m_keyframe; }
    void setKeyframe(const QByteArray &bytes) { m_keyframe = bytes; }

    QJsonObject toJson() const;

private:
    quint32 m_sessionId{0};
    Phase m_phase{Phase::Idle};
    QString m_phone;
    QString m_codec;
    QSize m_resolution{0, 0};
    QByteArray m_keyframe;
};
