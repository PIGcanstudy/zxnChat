#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H
#include <QLabel>
#include "global.h"
#include <QWidget>

class ClickedLabel:public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget* parent);
    // 鼠标的四种事件
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent* event) override;
    // 设置状态，可以从qss中获取对应形态
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");
    // 得到当前的状态
    ClickLbState GetCurState();

    bool SetCurState(ClickLbState state);
    void ResetNormalState();
protected:

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLbState _curstate;
signals:
    // 点击信号
    void clicked(QString, ClickLbState);

};

#endif // CLICKEDLABEL_H
