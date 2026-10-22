#include "VehicleJson.h"
#include "Protocol.h"
#include "VehicleModel.h"
#include <QDebug>
#include <QSet>

namespace VehicleJson {

QList<QMetaProperty> properties(const QMetaObject &meta)
{
    QList<QMetaProperty> result;
    for (int i = VehicleModel::staticMetaObject.propertyOffset(); i < meta.propertyCount(); ++i) {
        result.append(meta.property(i));
    }
    return result;
}

QList<QMetaMethod> eventSignals(const QMetaObject &meta)
{
    QSet<int> notifySignals;
    for (const QMetaProperty &prop : properties(meta)) {
        if (prop.hasNotifySignal()) {
            notifySignals.insert(prop.notifySignalIndex());
        }
    }

    QList<QMetaMethod> result;
    for (int i = VehicleModel::staticMetaObject.methodOffset(); i < meta.methodCount(); ++i) {
        const QMetaMethod method = meta.method(i);
        if (method.methodType() == QMetaMethod::Signal
            && method.parameterCount() == 0
            && !notifySignals.contains(method.methodIndex())) {
            result.append(method);
        }
    }
    return result;
}

QJsonObject toJson(const VehicleModel &model)
{
    QJsonObject values;
    for (const QMetaProperty &prop : properties(*model.metaObject())) {
        if (prop.isReadable()) {
            values.insert(QString::fromLatin1(prop.name()), QJsonValue::fromVariant(prop.read(&model)));
        }
    }
    return values;
}

int applyJson(VehicleModel &model, const QJsonObject &values)
{
    const QMetaObject &meta = *model.metaObject();
    const int ownOffset = VehicleModel::staticMetaObject.propertyOffset();
    int written = 0;

    model.setExactWrites(true);
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        const int index = meta.indexOfProperty(it.key().toLatin1().constData());
        if (index < ownOffset) {
            qWarning().noquote() << QStringLiteral("unknown vehicle key: %1").arg(Protocol::withoutControlChars(it.key()));
            continue;
        }

        const QMetaProperty prop = meta.property(index);
        if (!prop.isWritable()) continue;

        QVariant value = it.value().toVariant();
        if (!value.convert(prop.metaType())) {
            qWarning().noquote() << QStringLiteral("vehicle key %1: value not convertible").arg(Protocol::withoutControlChars(it.key()));
            continue;
        }
        if (prop.write(&model, value)) {
            ++written;
        }
    }
    model.setExactWrites(false);
    return written;
}

bool raiseEvent(VehicleModel &model, const QString &name)
{
    const QList<QMetaMethod> events = eventSignals(*model.metaObject());
    for (const QMetaMethod &method : events) {
        if (QString::fromLatin1(method.name()) == name) {
            return method.invoke(&model, Qt::DirectConnection);
        }
    }
    return false;
}

} // namespace VehicleJson
