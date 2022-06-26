//
// Created by jgravier on 6/26/22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SoundWidget.h" resolved

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QBuffer>
#include <QFile>

#include "../ui/ui_SoundWidget.h"
#include "SoundWidget.h"
#include "src/audio_decoder.h"

#define AUDIO_SAMPLE_RATE 11025
#define PLAY_ICON_NAME "media-playback-start"
#define PAUSE_ICON_NAME "media-playback-pause"

SoundWidget::SoundWidget(QWidget *parent) : AssetWidget(parent), ui(new Ui::SoundWidget)
{
  QAudioFormat audioFmt;
  audioFmt.setChannelCount(1);
  audioFmt.setSampleSize(8);
  audioFmt.setSampleRate(AUDIO_SAMPLE_RATE);
  audioFmt.setSampleType(QAudioFormat::UnSignedInt);

  QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
  if (!deviceInfo.isFormatSupported(audioFmt))
  {
    qWarning() << "Raw audio format not supported by backend, cannot play audio.";
  }
  else
  {
    qDebug() << "Audio will played using" << deviceInfo.deviceName();
  }

  ui->setupUi(this);

  audio = new QAudioOutput(audioFmt);
  audio->setNotifyInterval(10);
  currentBuffer = nullptr;

  connect(audio, &QAudioOutput::notify, this, &SoundWidget::updateSlider);
  connect(audio, &QAudioOutput::stateChanged, this, &SoundWidget::audioStateChanged);
  connect(ui->playPauseButton, &QPushButton::clicked, this, &SoundWidget::onPlayPauseClicked);
  connect(ui->stopButton, &QPushButton::clicked, this, &SoundWidget::onStopClicked);
}

SoundWidget::~SoundWidget()
{
  delete ui;
  delete audio;
}

void
SoundWidget::setAsset(const FileInfo *asset)
{
  if (currentBuffer != nullptr && currentBuffer->isOpen())
  {
    currentBuffer->close();
    delete currentBuffer;
  }

  if (audio->state() != QAudio::StoppedState)
  {
    audio->stop();
  }

  if (!decodedAudio.isEmpty())
  {
    decodedAudio.clear();
  }

  currentAsset = *asset;

  // TODO: Clean this decoding shit
  char *decoded = nullptr;
  uint decodedSize = decode_audio(static_cast<const uint8_t *>(currentAsset.data), decoded);
  decodedAudio.setRawData(reinterpret_cast<const char *>(decoded), decodedSize);
  ui->audioSlider->setMaximum(static_cast<int>(decodedSize));
}

void
SoundWidget::play()
{
  currentBuffer = new QBuffer(&decodedAudio);
  currentBuffer->open(QIODevice::ReadOnly);

  audio->start(currentBuffer);
}

void
SoundWidget::audioStateChanged(QAudio::State newState)
{
  // Playback is finished,
  // We reset to the beginning of the buffer
  if (newState == QAudio::IdleState)
  {
    audio->reset();
    ui->audioSlider->setValue(0);
  }

  if (newState != QAudio::ActiveState)
  {
    ui->playPauseButton->setIcon(QIcon::fromTheme(PLAY_ICON_NAME));
  }
  else
  {
    ui->playPauseButton->setIcon(QIcon::fromTheme(PAUSE_ICON_NAME));
  }

  if (newState == QAudio::StoppedState)
  {
    ui->audioSlider->setValue(0);
  }
}

void
SoundWidget::onPlayPauseClicked()
{
  switch (audio->state())
  {
  case QAudio::ActiveState:
    audio->suspend();
    break;

  case QAudio::IdleState:
  case QAudio::SuspendedState:
    audio->resume();
    break;
  case QAudio::StoppedState:
    play();
    break;
  }
}

void
SoundWidget::onStopClicked()
{
  audio->reset();
}

void
SoundWidget::updateSlider()
{
  ui->audioSlider->setValue(static_cast<int>(currentBuffer->pos()));
}
