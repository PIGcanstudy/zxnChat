#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include "global.h"

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void initHead();
    void initHttpHandlers();
    void showTip(QString str,bool b_ok);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    bool checkPwdValid();
    bool enableBtn(bool enabled);
    bool checkUserValid();

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    Ui::LoginDialog *ui;
    int _uid;
    QString _token;
public slots:
    void slot_forget_pwd();
signals:
    void SwitchRegister();
    //
    void SwitchReset();
    void sig_connect_tcp(ServerInfo);
private slots:
    void on_login_btn_clicked();
    // 登录模块的槽函数
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // LOGINDIALOG_H
