#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QAbstractItemView>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("vHost Generator"));

    QDir dir;
    QFile sFile(QDir::currentPath() + "/settings.ini");




    if (!sFile.exists())
    {
        QMessageBox::warning(this, "Error", "No settings file exists! Visit the settings page and configure the program!");
    }
    else
    {
        QSettings settings(sFile.fileName(), QSettings::IniFormat);

        settings.beginGroup("main");

        QFile confFile;
        confFile.setFileName(QDir::toNativeSeparators(settings.value("vhost").toString()));

        this->vhost = confFile.fileName();
        this->hosts = QDir::toNativeSeparators(settings.value("hosts").toString());

         // Make backups

        if (!confFile.exists())
        {
            QMessageBox::warning(this, "Error", "The specified vhost file doesn't exist! Make sure the file exists!", QMessageBox::Ok);
        }
        else
        {
           // Make a backup
           QFile confBak(QDir::toNativeSeparators(settings.value("vhost").toString().replace(".conf", ".conf.bak")));
           if (!confBak.exists()) {
               QFile::copy(this->vhost, confBak.fileName());
           }


           QFile hBak(QDir::toNativeSeparators(settings.value("hosts").toString()).replace("hosts", "hosts.bak"));

           if (!hBak.exists()) {

               QFile::copy(this->hosts, QDir::currentPath() + "/" + hBak.fileName()); // made a backup in our folder since we can't write into windows
           }

            this->parseXML(); // It seems apache conf files are not XML compliant :(
        }
    }

    model = new QStringListModel(this);
    //existingVHosts << "Example" << "EXAMPLE B" << "EXAMPLE C" << "EXAMPLE D";
    model->setStringList(existingVHosts);
    this->ui->existingHosts->setModel(model);
    this->ui->existingHosts->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this->ui->existingHosts->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(vhostHasChanged(QItemSelection)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::vhostHasChanged(QItemSelection s)
{
    this->activeSelectionIndex = s;
}
void MainWindow::parseXML()
{
    QFile *file = new QFile(this->vhost);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this,
                              "Error",
                              "Couldn't open .conf file",
                              QMessageBox::Ok);
        return;
    }
    //QRegExp vh("<VirtualHost");

  /*  QRegExp vhost("(<VirtualHost\\s:?[^>]*>.*?</VirtualHost>)");

    vhost.setPatternSyntax(QRegExp::W3CXmlSchema11);
    vhost.setMinimal(false);
    QString txt = QString(file->readAll());



    vhost.indexIn(txt);
   qDebug() << vhost.capturedTexts(); */

    // Since regex failed, let's do another trick.
    int lineNumber = 0;
    QStringList captures;

    while (!file->atEnd()) {
         QByteArray line = file->readLine();

         if (QString(line).contains("<VirtualHost"))
         {
            captures << QString(line).replace("<VirtualHost", "").replace(">", "").replace("\n", "").trimmed();
         }
         lineNumber++;
    }



    existingVHosts = captures;


}

void MainWindow::appendToFile(QString _file, QString content)
{
    QFile *file = new QFile(_file);
    file->open(QIODevice::Append);
    file->write(content.toLatin1());
    file->close();
}

void MainWindow::on_addButton_clicked()
{
    QString hostName = this->ui->hostName->text();
    QString folderName = this->ui->folderName->text();

    if (hostName.isEmpty() || folderName.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Host Name or Folder Name can not be empty!", QMessageBox::Ok);
        return;
    }

    QString host = "\n<VirtualHost "+hostName+":80>\n";
            host += "ServerName "+hostName+"\n";
            host += "DocumentRoot \""+folderName+"\" \n";
            host += "</VirtualHost>";

    this->appendToFile(this->vhost, host);
    this->appendToFile(this->hosts, "\n127.0.0.1 " + hostName);
    this->statusBar()->showMessage("vhost has been added");

    existingVHosts << hostName;
    model->setStringList(existingVHosts);

}

void MainWindow::on_settingsButton_clicked()
{

    Dialog dialogUi;
    dialogUi.exec();
}

QString MainWindow::getSettingsFilePath() const
{
    return QDir::currentPath() + "/settings.ini";
}

void MainWindow::on_deleteButton_clicked()
{


    if (!this->activeSelectionIndex.indexes().isEmpty())
    {
       QMessageBox::StandardButton question;
       question = QMessageBox::warning(this,  tr("Are you sure?"), tr("Do you really want to delete this vhost?"), QMessageBox::Ok | QMessageBox::Abort);
       if (question == QMessageBox::Ok)
       {



           int delIndex = this->activeSelectionIndex.indexes()[0].row();
           this->deleteVhost(delIndex);

           existingVHosts.removeAt(delIndex);
           model->removeRows(ui->existingHosts->currentIndex().row(), 1);


       }

    }

}

void MainWindow::deleteVhost(int delIndex)
{
    int lineNumber = 0;
    QFile *file = new QFile(this->vhost);

    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(this,
                              "Error",
                              "Couldn't open .conf file",
                              QMessageBox::Ok);
        return;
    }
    file->seek(0);

    QStringList captures;

    bool drop = false;

    QString newContent;

    while (!file->atEnd()) {
         QString line = file->readLine();

         if (line.contains(existingVHosts[delIndex])) // starting host tag
         {
            drop = true;
         }


         if (!drop)
         {
            newContent += line;
         }

         if (line.contains("</VirtualHost>")) // ending host tag
         {
             drop = false;
         }
         lineNumber++;
    }

    file->close();

    file->open(QIODevice::Truncate | QIODevice::ReadWrite); //clean it
    file->write(newContent.toLatin1(), newContent.size());
    file->close();

    QFile *h = new QFile(this->hosts);



    if (!h->open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(this,
                              "Error",
                              "Couldn't open hosts file",
                              QMessageBox::Ok);
        return;
    }

    h->seek(0);
    QString cc;
    while(!h->atEnd())
    {
        QString line = h->readLine();
        if (!line.contains(existingVHosts[delIndex].split(":")[0]))
        {
           cc += line;
        }
    }



    h->close();

    if (h->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        h->write(cc.toLatin1(), cc.size());
        h->close();
    }






}
