#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QDebug>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel();
    ui->treeView->setModel(model);

    widgets["SND"] = new QWidget(ui->frame);
    widgets["SND"]->setStyleSheet("background-color:red;");
    widgets["SND"]->hide();


//    QWidget *test = new QWidget(ui->frame);
//    test->setGeometry(ui->frame->frameRect());

//    test->show();

    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::SelectedAsset);
    connect(ui->actionOpen_File, &QAction::triggered, [=]{
//        QString filePath = QFileDialog::getOpenFileName(this, "Open resource file", QDir::homePath(), "Resource files (*.res)");
        QString filePath("/Users/jean/dev/perso/sammy-data-analysis/SAMMY0.RES");
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
    for(auto it = file->getAssets().begin(); it != file->getAssets().end(); ++it)
    {
        QString itemName = QString("%1.%2").arg(++i).arg(it->ext);
        QStandardItem *item = new QStandardItem(itemName);
        item->setData(QVariant::fromValue(*it));
        item->setEditable(false);
        rootItem->appendRow(item);
    }

    model->appendRow(rootItem);
}

void MainWindow::SelectedAsset(const QModelIndex &index)
{
    QStandardItem *item = model->itemFromIndex(index);
    FileInfo *asset = (FileInfo *)item->data().constData();

    QWidget *widget = widgets[asset->ext];
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
}

MainWindow::~MainWindow()
{
    delete ui;

    for(auto it = openResources.begin(); it != openResources.end(); ++it)
    {
        delete *it;
    }
}
