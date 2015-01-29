#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QStringListModel>
#include <QItemSelection>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString vhost;
    QString hosts;
    QString getSettingsFilePath() const;
    QStringListModel *model;
    QStringList existingVHosts;
    QItemSelection activeSelectionIndex;
    QSettings settingsData;

private slots:
    void on_exitButton_clicked();
    void parseXML();
    void appendToFile(QString,QString);
    void on_addButton_clicked();
    void vhostHasChanged(QItemSelection);
    void on_settingsButton_clicked();
    void deleteVhost(int);
    void on_deleteButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
