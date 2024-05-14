#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->pw_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    //一般配合css使用，设置一个东西的属性
    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);

    //连接注册模块
    connect(Httpmgr::GetInstance().get(),&Httpmgr::sig_reg_mod_finish,this, &RegisterDialog::slot_reg_mod_finish);

    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_clicked()
{
    qDebug()<<"receive varify btn clicked ";
    auto email = ui->email_edit->text();
    //用来使用正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w*)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match){
        //匹配的话发送验证码
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        //发送请求
        Httpmgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
        qDebug() << gate_url_prefix;
    }else{
        showTip(tr("邮箱地址不正确"), false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), false);
        return;
    }

    //解析JSON字符串，res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"), false);
        return;
    }

    //json解析了，但是格式错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"), false);
        return;
    }

    //调用对应的逻辑，根据id来调，因为根据id查的的是函数对象
    _handlers[id](jsonDoc.object());

    return;
}

void RegisterDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void RegisterDialog::initHttpHandlers()
{
    //注册回去验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            qDebug() << "服务端发送错误回包";
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送,请注意查收"), true);
        qDebug() << "email is" << email;
    });

    //...其他逻辑
}

