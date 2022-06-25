//
// Created by jgravier on 6/25/22.
//

#ifndef SAMMY_FILE_VIEWER_SPRITEWIDGET_H
#define SAMMY_FILE_VIEWER_SPRITEWIDGET_H

#include <QWidget>
#include "AssetWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SpriteWidget; }
QT_END_NAMESPACE

class SpriteWidget : public AssetWidget
{
public:
    explicit SpriteWidget(QWidget *parent = nullptr);
    ~SpriteWidget() override;

    void setAsset(const FileInfo *asset) override;
    void setSprite(const uint8_t *data, uint size);

private:
    Ui::SpriteWidget *ui;
//    QImage *img = nullptr;
};


#endif //SAMMY_FILE_VIEWER_SPRITEWIDGET_H
