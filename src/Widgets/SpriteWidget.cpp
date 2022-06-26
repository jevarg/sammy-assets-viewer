//
// Created by jgravier on 6/25/22.
//

#include "SpriteWidget.h"
#include "../ui/ui_SpriteWidget.h"

#define PALETTE_SIZE 256

SpriteWidget::SpriteWidget(QWidget *parent) : AssetWidget(parent), ui(new Ui::SpriteWidget), colorTable(PALETTE_SIZE)
{
  for (int i = 0; i < PALETTE_SIZE; i++)
  {
    colorTable[i] = qRgb(
      i * 4 % (PALETTE_SIZE - 1),
      i * 2 % (PALETTE_SIZE - 1),
      i * 3 % (PALETTE_SIZE - 1));
  }

  ui->setupUi(this);
  connect(ui->widthSlider, &QSlider::sliderMoved, this, &SpriteWidget::onWidthSliderChanged);
  connect(ui->widthSpinBox, &QSpinBox::valueChanged, this, &SpriteWidget::onWidthSpinBoxChanged);
  connect(ui->heightSlider, &QSlider::sliderMoved, this, &SpriteWidget::onHeightSliderChanged);
  connect(ui->heightSpinBox, &QSpinBox::valueChanged, this, &SpriteWidget::onHeightSpinBoxChanged);
}

SpriteWidget::~SpriteWidget()
{
  delete ui;
}

void
SpriteWidget::setAsset(const FileInfo *asset)
{
  currentAsset = *asset; // TODO: keep pointer instead

  int defaultWidth = 576;

  ui->widthSlider->setValue(defaultWidth);
  onWidthSliderChanged(defaultWidth);
}

void
SpriteWidget::updateImage()
{
  QImage img = QImage((uchar *)(currentAsset.data),
                      currentSize.width(),
                      currentSize.height(),
                      QImage::Format_Indexed8);

  img.setColorTable(colorTable);

  ui->label->setPixmap(QPixmap::fromImage(img));
  ui->label->show();
}

void
SpriteWidget::refreshSize()
{
  if (ui->widthSpinBox->value() != currentSize.width())
  {
    ui->widthSpinBox->blockSignals(true);
    ui->widthSpinBox->setValue(currentSize.width());
    ui->widthSpinBox->blockSignals(false);
  }

  if (ui->widthSlider->value() != currentSize.width())
  {
    ui->widthSlider->setValue(currentSize.width());
  }

  if (ui->heightSpinBox->value() != currentSize.height())
  {
    ui->heightSpinBox->blockSignals(true);
    ui->heightSpinBox->setValue(currentSize.height());
    ui->heightSpinBox->blockSignals(false);
  }

  if (ui->heightSlider->value() != currentSize.height())
  {
    ui->heightSlider->setValue(currentSize.height());
  }
}

///////////
// Slots //
///////////
void
SpriteWidget::onWidthSliderChanged(int w)
{
  currentSize.setWidth(w);
  currentSize.setHeight(currentAsset.size / w);

  refreshSize();
  updateImage();
}

void
SpriteWidget::onWidthSpinBoxChanged(int w)
{
  currentSize.setWidth(w);
  currentSize.setHeight(currentAsset.size / w);

  refreshSize();
  updateImage();
}

void
SpriteWidget::onHeightSliderChanged(int h)
{
  currentSize.setHeight(h);
  currentSize.setWidth(currentAsset.size / h);

  refreshSize();
  updateImage();
}

void
SpriteWidget::onHeightSpinBoxChanged(int h)
{
  currentSize.setHeight(h);
  currentSize.setWidth(currentAsset.size / h);

  refreshSize();
  updateImage();
}
