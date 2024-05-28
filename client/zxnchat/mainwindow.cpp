#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建登录界面
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);

    //实现登录界面与注册界面的切换，使用信号与槽来实现
    connect(_login_dlg, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchReg);

    //连接登录界面忘记密码信号
    connect(_login_dlg, &LoginDialog::SwitchReset, this, &MainWindow::SlotSwitchReset);

    //连接创建聊天界面信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_switch_chatdlg, this, &MainWindow::SlotSwitchChat);

    // 测试用
    emit TcpMgr::GetInstance()->sig_switch_chatdlg();
}

void MainWindow::SlotSwitchReg(){
    //创建注册界面
    _reg_dlg = new RegisterDialog(this);

    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    // 连接注册界面的返回登录信号
    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

//从注册界面返回登录界面
void MainWindow::SlotSwitchLogin()
{
    //创建登录界面
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);

    //实现登录界面与注册界面的切换，使用信号与槽来实现
    connect(_login_dlg, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchReg);

    //连接登录界面忘记密码信号
    connect(_login_dlg, &LoginDialog::SwitchReset, this, &MainWindow::SlotSwitchReset);
}

//从重置界面返回登录界面
void MainWindow::SlotResetSwitchLogin()
{
    //创建登录界面
    _login_dlg = new LoginDialog(this);
    // 最大最小化隐藏起来，还有边框
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);

    //实现登录界面与忘记密码界面的切换，使用信号与槽来实现
    connect(_login_dlg, &LoginDialog::SwitchReset, this, &MainWindow::SlotSwitchReset);

    //实现登录界面与注册界面的切换，使用信号与槽来实现
    connect(_login_dlg, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchReg);

}

void MainWindow::SlotSwitchReset()
{
    _reset_dlg = new ResetDialog(this);
    // 去除边界化
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    // 设置reset为主widget
    setCentralWidget(_reset_dlg);

    _login_dlg->hide();
    _reset_dlg->show();

    //返回登录界面信号和槽函数
    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotResetSwitchLogin);
}

void MainWindow::SlotSwitchChat()
{
    _chat_dlg = new ChatDialog();
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
    _login_dlg->hide();
    this->setMinimumSize(QSize(900,750));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

MainWindow::~MainWindow()
{
    delete ui;
}

