/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterMediaController.cpp
 * Author:         SK Rehan Ahamed
 * Description:    Cluster Media Controller Implementation
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#include "ClusterMediaController.h"
#include <vector>

struct MediaTrack {
    QString source;
    QString artist;
    QString title;
};

static const std::vector<MediaTrack> s_playlist = {
    { QString::fromUtf8("USB"), QString::fromUtf8("Revoic"), QString::fromUtf8("Sunset Drive") },
    { QString::fromUtf8("Bluetooth"), QString::fromUtf8("The Weeknd"), QString::fromUtf8("Blinding Lights (After Hours)") },
    { QString::fromUtf8("Apple CarPlay"), QString::fromUtf8("Dua Lipa"), QString::fromUtf8("Levitating (Club Future Nostalgia)") },
    { QString::fromUtf8("Android Auto"), QString::fromUtf8("A.R. Rahman"), QString::fromUtf8("Dil Se Re (Original Soundtrack)") },
    { QString::fromUtf8("Bluetooth"), QString::fromUtf8("Arijit Singh"), QString::fromUtf8("Kesariya (Brahmastra Audio)") },
    { QString::fromUtf8("USB"), QString::fromUtf8("Coldplay"), QString::fromUtf8("Viva La Vida") },
    { QString::fromUtf8("FM Radio"), QString::fromUtf8("Radio Mirchi 98.3 FM"), QString::fromUtf8("Top Bollywood Hits Live") }
};

ClusterMediaController::ClusterMediaController(QObject *parent)
    : QObject(parent)
{
    m_mediaPopupTimer.setSingleShot(true);
    connect(&m_mediaPopupTimer, &QTimer::timeout, this, &ClusterMediaController::onMediaPopupTimeout);
}

void ClusterMediaController::setShowMediaPopup(bool show)
{
    if (m_showMediaPopup != show) {
        m_showMediaPopup = show;
        emit showMediaPopupChanged();
    }
}

void ClusterMediaController::setMediaSource(const QString &source)
{
    if (m_mediaSource != source) {
        m_mediaSource = source;
        emit mediaSourceChanged();
    }
}

void ClusterMediaController::setMediaTitle(const QString &title)
{
    if (m_mediaTitle != title) {
        m_mediaTitle = title;
        emit mediaTitleChanged();
    }
}

void ClusterMediaController::setMediaArtist(const QString &artist)
{
    if (m_mediaArtist != artist) {
        m_mediaArtist = artist;
        emit mediaArtistChanged();
    }
}

void ClusterMediaController::setIsMediaPlaying(bool playing)
{
    if (m_isMediaPlaying != playing) {
        m_isMediaPlaying = playing;
        emit isMediaPlayingChanged();
    }
}

void ClusterMediaController::setMediaTrackIndex(int index)
{
    if (m_mediaTrackIndex != index) {
        m_mediaTrackIndex = index;
        emit mediaTrackIndexChanged();
    }
}

void ClusterMediaController::onMediaPopupTimeout()
{
    setShowMediaPopup(false);
}

void ClusterMediaController::triggerMediaPopup()
{
    setShowMediaPopup(true);
    m_mediaPopupTimer.start(5000);
}

void ClusterMediaController::dismissMediaPopup()
{
    if (m_mediaPopupTimer.isActive()) {
        m_mediaPopupTimer.stop();
    }
    setShowMediaPopup(false);
}

void ClusterMediaController::playTrack(const QString &source, const QString &artist, const QString &title)
{
    setMediaSource(source);
    setMediaArtist(artist);
    setMediaTitle(title);
    setIsMediaPlaying(true);
    triggerMediaPopup();
}

void ClusterMediaController::nextMediaTrack()
{
    m_mediaTrackIndex = static_cast<int>((m_mediaTrackIndex + 1) % s_playlist.size());
    emit mediaTrackIndexChanged();
    const auto &track = s_playlist[m_mediaTrackIndex];
    playTrack(track.source, track.artist, track.title);
}

void ClusterMediaController::prevMediaTrack()
{
    m_mediaTrackIndex = static_cast<int>((m_mediaTrackIndex - 1 + s_playlist.size()) % s_playlist.size());
    emit mediaTrackIndexChanged();
    const auto &track = s_playlist[m_mediaTrackIndex];
    playTrack(track.source, track.artist, track.title);
}

void ClusterMediaController::toggleMediaPlayback()
{
    setIsMediaPlaying(!m_isMediaPlaying);
    triggerMediaPopup();
}
