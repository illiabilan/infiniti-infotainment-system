#pragma once

#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QMetaProperty>
#include <QObject>
#include <QString>

class VehicleModel;

// Reports VehicleModel changes generically through QMetaObject, so no
// per-property connect is needed. The model must outlive the watcher.
class VehicleChangeWatcher : public QObject
{
    Q_OBJECT

public:
    explicit VehicleChangeWatcher(VehicleModel *model, QObject *parent = nullptr);

signals:
    void valuesChanged(const QJsonObject &values);
    void eventRaised(const QString &name);

private slots:
    void onSignal();

private:
    VehicleModel *m_model;
    QHash<int, QList<QMetaProperty>> m_propertiesBySignal;
    QHash<int, QString> m_eventBySignal;
};
