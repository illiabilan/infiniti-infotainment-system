#pragma once

#include "CanFrame.h"
#include "VehicleModel.h"

class CanInterface;

// Binds the vehicle model to a CAN bus. Core creates exactly one (CL-1).
class VehicleState : public VehicleModel
{
    Q_OBJECT

public:
    explicit VehicleState(QObject *parent = nullptr);
    ~VehicleState() override;

    void attachCanInterface(CanInterface *canInterface);
    CanInterface* canInterface() const { return m_canInterface; }

    void publish(VehicleModel::Group group, quint8 commandAction = 0) override;

public slots:
    void processFrame(const CanFrame &frame);

private:
    CanInterface *m_canInterface{nullptr};
};
