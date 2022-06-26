//
// Created by jgravier on 6/25/22.
//

#pragma once

#include "AssetWidget.h"
#include <QPainter>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SpriteWidget;
}
QT_END_NAMESPACE

class SpriteWidget : public AssetWidget
{
public:
  explicit SpriteWidget(QWidget *parent = nullptr);
  ~SpriteWidget() override;

  void setAsset(const FileInfo *asset) override;

private:
  Ui::SpriteWidget *ui;
  FileInfo currentAsset;
  QSize currentSize;
  QVector<QRgb> colorTable;

  void refreshSize();
  void updateImage();

private slots:
  void onWidthSliderChanged(int w);
  void onWidthSpinBoxChanged(int w);
  void onHeightSliderChanged(int h);
  void onHeightSpinBoxChanged(int h);
};
