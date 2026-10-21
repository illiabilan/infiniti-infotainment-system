#pragma once

#include <QJsonObject>
#include <QList>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QString>

class VehicleModel;

// Reflection-based JSON mapping of the VehicleModel properties and events.
namespace VehicleJson {

// Properties declared by VehicleModel and its subclasses (excludes QObject::objectName)
QList<QMetaProperty> properties(const QMetaObject &meta);

// Parameterless signals that are not the notify signal of a property
QList<QMetaMethod> eventSignals(const QMetaObject &meta);

QJsonObject toJson(const VehicleModel &model);

// Writes through the setters with exact change detection (no 0.1 dedupe). Returns the number of successful writes.
int applyJson(VehicleModel &model, const QJsonObject &values);

// Emits an allow-listed event signal by name. Returns false if the name is not an event.
bool raiseEvent(VehicleModel &model, const QString &name);

} // namespace VehicleJson
