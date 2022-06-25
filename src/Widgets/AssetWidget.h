//
// Created by jgravier on 6/25/22.
//

#ifndef SAMMY_FILE_VIEWER_ASSETWIDGET_H
#define SAMMY_FILE_VIEWER_ASSETWIDGET_H

#include <QWidget>
#include "src/ResourceFile.h"

class AssetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssetWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual void setAsset(const FileInfo *asset) = 0;
};

#endif //SAMMY_FILE_VIEWER_ASSETWIDGET_H
