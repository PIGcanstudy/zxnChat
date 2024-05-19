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

    // 清空错误输出框
    ui->err_tip->clear();

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->pw_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });

    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });

    //设置浮动显示手形状
    ui->pw_visable->setCursor(Qt::PointingHandCursor);
    ui->confirm_visable->setCursor(Qt::PointingHandCursor);

    ui->pw_visable->SetState("unvisible","unvisible_hover","","visible",
                               "visible_hover","");

    ui->confirm_visable->SetState("unvisible","unvisible_hover","","visible",
                                  "visible_hover","");
    //连接点击事件

    connect(ui->pw_visable, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pw_visable->GetCurState();
        if(state == ClickLbState::Normal){
            ui->pw_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->pw_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    connect(ui->confirm_visable, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visable->GetCurState();
        if(state == ClickLbState::Normal){
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    // 初始化定时器
    _countdown_timer = new QTimer(this);

    // 连接信号和槽
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if(_countdown <= 0){
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown --;
        auto str = QString("注册成功，%1 s后返回登录").arg(_countdown);
        ui->tip_label->setText(str);
    });
}

RegisterDialog::~RegisterDialog()
{
    qDebug() << "RegisterDialog destruce!";
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

    //注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug() << "user uuid is " << jsonObj["uid"].toString();
        qDebug()<< "email is " << email ;
        ChangeTipPage();
    });

    //...其他逻辑
}


void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    showTip(_tip_errs.first(), false);
}

void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    // 启动定时器，设置事件间隔为1000ms
    _countdown_timer->start(1000);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pw_edit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");

    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    auto pass = ui->varify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->pw_edit->text();
    auto confirm = ui->confirm_edit->text();

    if(confirm.length() < 6 || confirm.length() > 15 ){
        //提示长度不准确
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(confirm).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_CONFIRM_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码和密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::checkAllVaild(){
    bool valid = checkUserValid();
    if(!valid){
        return false;
    }

    valid = checkEmailValid();
    if(!valid){
        return false;
    }

    valid = checkPassValid();
    if(!valid){
        return false;
    }

    valid = checkVarifyValid();
    if(!valid){
        return false;
    }

    valid = checkConfirmValid();
    if(!valid){
        return false;
    }

    return true;
}
void RegisterDialog::on_sure_btn_clicked()
{

    if(!checkAllVaild()) return;

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pw_edit->text());
    json_obj["confirm"] = xorString(ui->confirm_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();

    qDebug() << "发送数据给服务器";
    Httpmgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}


void RegisterDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

