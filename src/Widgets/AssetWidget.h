//
// Created by jgravier on 6/25/22.
//

#pragma once

#include "src/ResourceFile.h"
#include <QWidget>

class AssetWidget : public QWidget
{
  Q_OBJECT

public:
  explicit AssetWidget(QWidget *parent = nullptr) : QWidget(parent) {}
  virtual void setAsset(const FileInfo *asset) = 0;
};
