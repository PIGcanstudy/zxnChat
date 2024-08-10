#include "timerbtn.h"
#include <QMouseEvent>


TimerBtn::TimerBtn(QWidget *parent):QPushButton(parent), _counter(60)
{

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, [this](){
        _counter--;
        if(_counter <= 0){
            // 停止计时
            _timer->stop();
            _counter = 60;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(_counter));
    });
}

TimerBtn::~TimerBtn()
{
    _timer->stop();
}

//处理鼠标左键的释放事件
void TimerBtn::mouseReleaseEvent(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton){
        qDebug() << "MyButton was released!";
        this->setEnabled(false);
        this->setText(QString::number(_counter));
        // 计时1s
        _timer->start(1000);
        // 发送点击信号
        emit clicked();
    }

    // 调用基类的mouseReleaseEvent以确保正常的事件处理（如点击效果）
    QPushButton::mouseReleaseEvent(e);
}
