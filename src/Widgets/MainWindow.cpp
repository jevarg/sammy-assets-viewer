#include "MainWindow.h"
#include "../ui/ui_MainWindow.h"
#include "SoundWidget.h"
#include "SpriteWidget.h"

#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  model = new QStandardItemModel();
  ui->treeView->setModel(model);

  QVBoxLayout *frameLayout = new QVBoxLayout();

  SpriteWidget *spriteWidget = new SpriteWidget(ui->frame);
  spriteWidget->hide();
  frameLayout->addWidget(spriteWidget);
  widgets["SPR"] = spriteWidget;

  SoundWidget *soundWidget = new SoundWidget(ui->frame);
  soundWidget->hide();
  frameLayout->addWidget(soundWidget);
  widgets["SND"] = soundWidget;

  ui->frame->setLayout(frameLayout);

  connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::SelectedAsset);
  connect(ui->actionOpen_File, &QAction::triggered, [=] {
    QString filePath = QFileDialog::getOpenFileName(this, "Open resource file", QDir::homePath(), "Resource files (*.res *.RES);;All files(*)");
    qDebug() << filePath;
    if (filePath.isEmpty())
      {
        return;
      }

    ReadResource(filePath);
  });
}

MainWindow::~MainWindow()
{
  delete ui->frame->layout();
  delete ui;

  for (auto &w : widgets)
  {
    delete w.second;
  }

  for (auto &resource : openResources)
  {
    delete resource;
  }
}

void
MainWindow::ReadResource(const QString &path)
{
  ResourceFile *file = new ResourceFile(path.toStdString());
  openResources.push_back(file);
  QStandardItem *rootItem = new QStandardItem(path);
  rootItem->setEditable(false);

  uint i = 0;
  for (const auto &asset : file->getAssets())
  {
    QString itemName = QString("%1.%2").arg(++i).arg(asset.ext);
    QStandardItem *item = new QStandardItem(itemName);
    item->setData(QVariant::fromValue(asset));
    item->setEditable(false);
    rootItem->appendRow(item);
  }

  model->appendRow(rootItem);
}

void
MainWindow::SelectedAsset(const QModelIndex &index)
{
  QStandardItem *item = model->itemFromIndex(index);
  const FileInfo *asset = (FileInfo *)item->data().constData();

  AssetWidget *widget = widgets[asset->ext];
  if (widget == nullptr)
  {
    if (shownWidget != nullptr)
    {
      shownWidget->hide();
      shownWidget = nullptr;
    }

    qDebug() << QString("No widget for asset type: %1").arg(asset->ext);
    return;
  }

  if (shownWidget != widget)
  {
    if (shownWidget != nullptr)
      shownWidget->hide();

    shownWidget = widget;
    shownWidget->show();
  }

  widget->setAsset(asset);
}
