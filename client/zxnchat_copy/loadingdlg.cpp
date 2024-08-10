#include "loadingdlg.h"
#include "ui_loadingdlg.h"
#include <QMovie>

LoadingDlg::LoadingDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDlg)
{
    ui->setupUi(this);
    // 不显示边框，菜单，让它一直处于最上层，类似于一个Widget
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    // 获取屏幕尺寸
    setFixedSize(parent->size());


    QMovie *movie = new QMovie(":/res/loading.gif");
    ui->loading_lb->setMovie(movie);
    movie->start();
}

LoadingDlg::~LoadingDlg()
{
    delete ui;
}
