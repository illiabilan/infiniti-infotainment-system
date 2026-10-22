/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterMediaController.h
 * Author:         SK Rehan Ahamed
 * Description:    Cluster Media Controller Header
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#pragma once
#include <QObject>
#include <QString>
#include <QTimer>

class ClusterMediaController : public QObject
{
    Q_OBJECT

public:
    explicit ClusterMediaController(QObject *parent = nullptr);

    bool showMediaPopup() const { return m_showMediaPopup; }
    QString mediaSource() const { return m_mediaSource; }
    QString mediaTitle() const { return m_mediaTitle; }
    QString mediaArtist() const { return m_mediaArtist; }
    bool isMediaPlaying() const { return m_isMediaPlaying; }
    int mediaTrackIndex() const { return m_mediaTrackIndex; }

    void setShowMediaPopup(bool show);
    void setMediaSource(const QString &source);
    void setMediaTitle(const QString &title);
    void setMediaArtist(const QString &artist);
    void setIsMediaPlaying(bool playing);
    void setMediaTrackIndex(int index);

    void triggerMediaPopup();
    void dismissMediaPopup();
    void playTrack(const QString &source, const QString &artist, const QString &title);
    void nextMediaTrack();
    void prevMediaTrack();
    void toggleMediaPlayback();

signals:
    void showMediaPopupChanged();
    void mediaSourceChanged();
    void mediaTitleChanged();
    void mediaArtistChanged();
    void isMediaPlayingChanged();
    void mediaTrackIndexChanged();

private slots:
    void onMediaPopupTimeout();

private:
    bool m_showMediaPopup = false;
    QString m_mediaSource = "USB";
    QString m_mediaTitle = "Sunset Drive";
    QString m_mediaArtist = "Revoic";
    bool m_isMediaPlaying = true;
    int m_mediaTrackIndex = 0;
    QTimer m_mediaPopupTimer;
};
