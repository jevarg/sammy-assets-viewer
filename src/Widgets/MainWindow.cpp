#include "MainWindow.h"
#include "SpriteWidget.h"
#include "../ui/ui_MainWindow.h"

#include <QDebug>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel();
    ui->treeView->setModel(model);

//    widgets["SND"] = new QWidget(ui->frame);
//    widgets["SND"]->setStyleSheet("background-color:red;");
//    widgets["SND"]->hide();

    widgets["SPR"] = new SpriteWidget(ui->frame);
    widgets["SPR"]->setStyleSheet("background-color: blue;");
    widgets["SPR"]->hide();


    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::SelectedAsset);
    connect(ui->actionOpen_File, &QAction::triggered, [=]{
        QString filePath = QFileDialog::getOpenFileName(this, "Open resource file", QDir::homePath(), "Resource files (*.res)");
//        QString filePath("/home/jgravier/dev/perso/sammy-data-analysis/SAMMY0.RES");
        qDebug() << filePath;
        if (filePath.isEmpty())
        {
            return;
        }

        ReadResource(filePath);
    });
}

void MainWindow::ReadResource(const QString &path)
{
    ResourceFile *file = new ResourceFile(path.toStdString());
    openResources.push_back(file);
    QStandardItem *rootItem = new QStandardItem(path);
    rootItem->setEditable(false);

    uint i = 0;
    for(const auto & asset : file->getAssets())
    {
        QString itemName = QString("%1.%2").arg(++i).arg(asset.ext);
        QStandardItem *item = new QStandardItem(itemName);
        item->setData(QVariant::fromValue(asset));
        item->setEditable(false);
        rootItem->appendRow(item);
    }

    model->appendRow(rootItem);
}

void MainWindow::SelectedAsset(const QModelIndex &index)
{
    QStandardItem *item = model->itemFromIndex(index);
    FileInfo *asset = (FileInfo *)item->data().constData();

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

MainWindow::~MainWindow()
{
    delete ui;

    for(auto & resource : openResources)
    {
        delete resource;
    }
}
