//
// Created by jgravier on 6/25/22.
//

#include "SpriteWidget.h"
#include "../ui/ui_SpriteWidget.h"

SpriteWidget::SpriteWidget(QWidget *parent) : AssetWidget(parent), ui(new Ui::SpriteWidget)
{
    ui->setupUi(this);
}

SpriteWidget::~SpriteWidget() {
    delete ui;
}

void SpriteWidget::setSprite(const uint8_t *data, uint size)
{
    QImage img = QImage(data, 576, (size / 576), QImage::Format_Indexed8);
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void SpriteWidget::setAsset(const FileInfo *asset)
{
    setSprite(static_cast<const uint8_t *>(asset->data), asset->size);
}
