#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include "xml.h"

QT_USE_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:

 //   Settings settings() const;

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void setSettings();
    Settings currentSettings;

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::SettingsDialog *ui;
    QIntValidator *intValidator;


signals:
      void window_unloaded();
};

#endif // SETTINGSDIALOG_H
