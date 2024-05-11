#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkReply>
/* 这些都已经包含到global.h里了
//QT的网络，需要在qmake或者cmake中加network
*   //网络管理
*   #include <QNetworkAccessManager>
*   //QT的json序列化
*   #include <QJsonObject>
*   //QT的json反序列化
*   #include <QJsonDocument>
*/



//想要拥有信号和曹的功能的继承QObject
//这就是CRTP的设计模式：给模板基类传递派生类的类型
class Httpmgr:public QObject, public Singleton<Httpmgr>
    ,public std::enable_shared_from_this<Httpmgr>
{
    Q_OBJECT

public:
    ~Httpmgr();
    //发送请求的函数
    //需要四个参数，第一个参数为请求路径url，第二个参数为序列化为json的请求数据
    //第三个参数为请求的id，第四个参数为请求来自哪个模块
    //后两个参数主要是为了在异步调用http的回调函数的时候
    //知道该请求是来自哪的，是什么类型的
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
private:
    //由于基类要用到派生类的私有构造函数，所以需要声明友元
    friend class Singleton<Httpmgr>;
    Httpmgr();
    //http管理器
    QNetworkAccessManager _manager;
private slots:
    void slot_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod);
signals:
    //当发送请求完成时通知所用到的信号
    //需要四个参数，第一个参数为请求类型id，第二个为反序列化后的数据
    //第三个为错误码，第四个为模块类型
    void sig_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod);

    //注册模块http请求完成发送此信号
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    //...其他模块的完成时发送的信号
};

#endif // HTTPMGR_H
