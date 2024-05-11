#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建登录界面
    _login_dlg = new LoginDialog(this);
    setCentralWidget(_login_dlg);

    //实现登录界面与注册界面的切换，使用信号与槽来实现
    connect(_login_dlg, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchReg);
    //创建注册界面
    _reg_dlg = new RegisterDialog(this);

    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _reg_dlg->hide();
}

void MainWindow::SlotSwitchReg(){
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

