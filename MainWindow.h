#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ResourceFile.h"

#include <string>
#include <QFileDialog>
#include <QMainWindow>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SelectedAsset(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QWidget *shownWidget = nullptr;

    std::vector<ResourceFile *> openResources;
    std::map<std::string, QWidget *> widgets;

    void ReadResource(const QString &path);
};
#endif // MAINWINDOW_H
