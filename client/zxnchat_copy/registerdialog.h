#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
    // 检测是否全部有效
    bool checkAllVaild();

private slots:
    void on_get_code_clicked();

    void on_sure_btn_clicked();

    void on_return_btn_clicked();

    void on_cancel_btn_clicked();

public slots:
    //接收httpmgr发送过来的注册信号的槽
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

signals:
    void sigSwitchLogin();

private:
    Ui::RegisterDialog *ui;

    void showTip(QString str, bool b_ok);
    // 初始化注册处理器
    void initHttpHandlers();

    // 缓存错误到map里
    void AddTipErr(TipErr te,QString tips);
    // 删除错误到map里
    void DelTipErr(TipErr te);
    // 改变tip的内容
    void ChangeTipPage();


    // 检测用户行是否有效
    bool checkUserValid();
    // 检测邮箱行是否有效
    bool checkEmailValid();
    // 检测密码行是否有效
    bool checkPassValid();
    // 检测验证码行是否有效
    bool checkVarifyValid();
    // 检测确认密码行行是否有效
    bool checkConfirmValid();

    // 模块码与模块处理函数
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

    // 错误码与对应内容
    // _tip_errs用来缓存各个输入框输入完成后提示的错误，
    // 如果该输入框错误清除后就显示剩余的错误，每次只显示一条
    QMap<TipErr, QString> _tip_errs;

    //计时器
    QTimer *_countdown_timer;
    int _countdown;
};

#endif // REGISTERDIALOG_H
