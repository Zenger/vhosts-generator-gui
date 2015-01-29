#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>

#include <QSettings>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QFile f(QDir::currentPath() + "/settings.ini");
    QSettings settings(f.fileName(), QSettings::IniFormat);

    this->ui->apachePath->setText(settings.value("main/vhost").toString());
    this->ui->hostsPath->setText(settings.value("main/hosts").toString());
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_dialogCancel_clicked()
{
    this->close();
}

void Dialog::on_browseApache_clicked()
{
    this->ui->apachePath->setText( QDir::toNativeSeparators( QFileDialog::getOpenFileName(this, tr("vhosts File"), "", tr("Apache configuration file (*.conf)")) ) );
}

void Dialog::on_browseHosts_clicked()
{
    this->ui->hostsPath->setText( QDir::toNativeSeparators( QFileDialog::getOpenFileName(this, tr("hosts File"), "C:\\Windows\\System32\\drivers\\etc", tr("Hosts file (*.*)")) ) );
}

void Dialog::on_dialogSave_clicked()
{

    QFile f(QDir::currentPath() + "/settings.ini");
    QSettings settings(f.fileName(), QSettings::IniFormat);

    settings.setValue("main/vhost", this->ui->apachePath->text());
    settings.setValue("main/hosts", this->ui->hostsPath->text());

    this->close();
}
