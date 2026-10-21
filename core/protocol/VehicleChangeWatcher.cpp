#include "VehicleChangeWatcher.h"
#include "VehicleJson.h"
#include "VehicleModel.h"

VehicleChangeWatcher::VehicleChangeWatcher(VehicleModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    const QMetaMethod slot = metaObject()->method(metaObject()->indexOfSlot("onSignal()"));

    for (const QMetaProperty &prop : VehicleJson::properties(*m_model->metaObject())) {
        if (!prop.hasNotifySignal()) continue;

        const int signalIndex = prop.notifySignalIndex();
        const bool alreadyConnected = m_propertiesBySignal.contains(signalIndex);
        m_propertiesBySignal[signalIndex].append(prop);
        if (!alreadyConnected) {
            connect(m_model, prop.notifySignal(), this, slot);
        }
    }

    for (const QMetaMethod &method : VehicleJson::eventSignals(*m_model->metaObject())) {
        m_eventBySignal[method.methodIndex()] = QString::fromLatin1(method.name());
        connect(m_model, method, this, slot);
    }
}

void VehicleChangeWatcher::onSignal()
{
    const int index = senderSignalIndex();

    const auto propertiesIt = m_propertiesBySignal.constFind(index);
    if (propertiesIt != m_propertiesBySignal.constEnd()) {
        QJsonObject values;
        for (const QMetaProperty &prop : propertiesIt.value()) {
            values.insert(QString::fromLatin1(prop.name()), QJsonValue::fromVariant(prop.read(m_model)));
        }
        emit valuesChanged(values);
        return;
    }

    const auto eventIt = m_eventBySignal.constFind(index);
    if (eventIt != m_eventBySignal.constEnd()) {
        emit eventRaised(eventIt.value());
    }
}
