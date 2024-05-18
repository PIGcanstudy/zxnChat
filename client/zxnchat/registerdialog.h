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

private slots:
    void on_get_code_clicked();

    void on_sure_btn_clicked();

public slots:
    //接收httpmgr发送过来的注册信号的槽
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    Ui::RegisterDialog *ui;
    void showTip(QString str, bool b_ok);
    // 初始化注册处理器
    void initHttpHandlers();
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
};

#endif // REGISTERDIALOG_H
