#include "mainwindow.h"
#include <QFile>
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qDebug() << "123";
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("open failed!");
    }

    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();

    QDir dir(app_path);

    app_path = dir.absolutePath();
    // 拼接文件名
    QString fileName = "config.ini";
    //设置路径
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator()+ fileName);

    QSettings settings(config_path, QSettings::IniFormat);

    QString gate_host = settings.value("GateServer/host").toString();

    QString gate_port = settings.value("GateServer/port").toString();

    gate_url_prefix = "http://"+ gate_host + ":" + gate_port;

    qDebug() << "Config path: " << config_path;
    qDebug() << "Gate host: " << gate_host;
    qDebug() << "Gate port: " << gate_port;


    MainWindow w;
    w.show();
    return a.exec();
}