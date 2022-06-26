//
// Created by jgravier on 6/26/22.
//

#pragma once

#include "AssetWidget.h"
#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QAudioOutput>
#include <QBuffer>
#include <QMediaPlayer>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SoundWidget;
}
QT_END_NAMESPACE

class SoundWidget : public AssetWidget
{
  Q_OBJECT

public:
  explicit SoundWidget(QWidget *parent = nullptr);
  ~SoundWidget() override;
  void setAsset(const FileInfo *asset) override;

private:
  Ui::SoundWidget *ui;
  QAudioOutput *audio;
  FileInfo currentAsset;
  QByteArray decodedAudio;
  QBuffer *currentBuffer;

  void play();

private slots:
  void updateSlider();
  void audioStateChanged(QAudio::State newState);
  void onPlayPauseClicked();
  void onStopClicked();
};
