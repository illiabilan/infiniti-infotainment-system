/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterTelltaleController.h
 * Author:         SK Rehan Ahamed
 * Description:    Telltales, Lighting, and TPMS Controller Header
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#pragma once
#include <QObject>
#include <QString>
#include <QTimer>

class ClusterTelltaleController : public QObject
{
    Q_OBJECT

public:
    explicit ClusterTelltaleController(QObject *parent = nullptr);

    bool parkBrakeActive() const { return m_parkBrakeActive; }
    bool seatbeltActive() const { return m_seatbeltActive; }
    bool absActive() const { return m_absActive; }
    bool tpmsActive() const { return m_tpmsActive; }
    bool batteryActive() const { return m_batteryActive; }
    bool oilActive() const { return m_oilActive; }
    bool leftIndicator() const { return m_leftIndicator; }
    bool rightIndicator() const { return m_rightIndicator; }
    bool highBeam() const { return m_highBeam; }
    bool lowBeam() const { return m_lowBeam; }
    bool positionLamp() const { return m_positionLamp; }
    bool masterWarning() const { return m_masterWarning; }
    bool lightWarning() const { return m_lightWarning; }
    bool escActive() const { return m_escActive; }
    bool escOffActive() const { return m_escOffActive; }
    bool checkEngineActive() const { return m_checkEngineActive; }
    bool smartKeyActive() const { return m_smartKeyActive; }
    bool steeringActive() const { return m_steeringActive; }
    bool airbagActive() const { return m_airbagActive; }

    void setParkBrakeActive(bool active);
    void setSeatbeltActive(bool active);
    void setAbsActive(bool active);
    void setTpmsActive(bool active);
    void setBatteryActive(bool active);
    void setOilActive(bool active);
    void setLeftIndicator(bool active);
    void setRightIndicator(bool active);
    void setHighBeam(bool active);
    void setLowBeam(bool active);
    void setPositionLamp(bool active);
    void setMasterWarning(bool active);
    void setLightWarning(bool active);
    void setEscActive(bool active);
    void setEscOffActive(bool active);
    void setCheckEngineActive(bool active);
    void setSmartKeyActive(bool active);
    void setSteeringActive(bool active);
    void setAirbagActive(bool active);

    void setAllTelltales(bool active);
    void setBulbCheckTelltales(bool active);
    void applyNormalTripTelltales();

    int lightMode() const { return m_lightMode; }
    void setLightMode(int mode);
    void cycleLightMode();
    bool showLightPopup() const { return m_showLightPopup; }
    void setShowLightPopup(bool show);

    double flPsi() const { return m_flPsi; }
    double frPsi() const { return m_frPsi; }
    double rlPsi() const { return m_rlPsi; }
    double rrPsi() const { return m_rrPsi; }
    bool tpmsCalibrated() const { return m_tpmsCalibrated; }

    void setFlPsi(double psi);
    void setFrPsi(double psi);
    void setRlPsi(double psi);
    void setRrPsi(double psi);
    void setAllPsi(double psi);
    void adjustPsi(const QString &tyre, double delta);
    void resetTpms();
    void setTpmsCalibrated(bool cal);

    void startTpmsDismissTimer();

signals:
    void parkBrakeActiveChanged();
    void seatbeltActiveChanged();
    void absActiveChanged();
    void tpmsActiveChanged();
    void batteryActiveChanged();
    void oilActiveChanged();
    void leftIndicatorChanged();
    void rightIndicatorChanged();
    void highBeamChanged();
    void lowBeamChanged();
    void positionLampChanged();
    void masterWarningChanged();
    void lightWarningChanged();
    void escActiveChanged();
    void escOffActiveChanged();
    void checkEngineActiveChanged();
    void smartKeyActiveChanged();
    void steeringActiveChanged();
    void airbagActiveChanged();

    void lightModeChanged();
    void showLightPopupChanged();

    void flPsiChanged();
    void frPsiChanged();
    void rlPsiChanged();
    void rrPsiChanged();
    void tpmsCalibratedChanged();
    void tpmsDismissTimeout();

private slots:
    void onLightPopupTimeout();
    void onTpmsDismissTimeout();

private:
    void updateTyrePsi(double &member, double psi, void (ClusterTelltaleController::*changedSignal)());
    void updateTpmsLowWarning();

    bool m_parkBrakeActive = true;
    bool m_seatbeltActive = true;
    bool m_absActive = false;
    bool m_tpmsActive = false;
    bool m_batteryActive = true;
    bool m_oilActive = true;
    bool m_leftIndicator = false;
    bool m_rightIndicator = false;
    bool m_highBeam = false;
    bool m_lowBeam = false;
    bool m_positionLamp = false;
    bool m_masterWarning = false;
    bool m_lightWarning = false;
    bool m_escActive = false;
    bool m_escOffActive = false;
    bool m_checkEngineActive = false;
    bool m_smartKeyActive = true;
    bool m_steeringActive = true;
    bool m_airbagActive = false;

    int m_lightMode = 0; // 0 = OFF, 1 = AUTO, 2 = POSITION, 3 = HEADLIGHT
    bool m_showLightPopup = false;
    QTimer m_lightPopupTimer;

    double m_flPsi = 35.0;
    double m_frPsi = 35.0;
    double m_rlPsi = 35.0;
    double m_rrPsi = 31.0;
    bool m_tpmsCalibrated = false;
    QTimer m_tpmsDismissTimer;
};
