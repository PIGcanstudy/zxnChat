#include "httpmgr.h"

Httpmgr::~Httpmgr()
{

}

Httpmgr::Httpmgr()
{
    //连接http请求和完成信号，信号槽机制保证队列消费
    connect(this, &Httpmgr::sig_http_finish, this, &Httpmgr::slot_http_finish);
}

void Httpmgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    // 将传入的 JSON 对象转换为字节数组
    QByteArray data = QJsonDocument(json).toJson();

    // 创建一个网络请求对象，并设置请求的 URL 和内容类型
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    // 用 shared_from_this() 获取当前对象的 shared_ptr，以避免在 lambda 表达式中使用 this 指针
    auto self = shared_from_this();

    // 发送 POST 请求，并获取响应的网络回复对象
    QNetworkReply * reply = _manager.post(request, data);

    // 使用 lambda 表达式连接网络回复的 finished 信号，当网络操作完成时执行相应的处理
    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod](){
        // 处理错误的情况：如果网络回复的错误代码不是 NoError，则表示发生了错误
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();

            // 发送信号通知完成，并传递错误代码和模块信息
            emit self->sig_http_finish(req_id,"", ErrorCodes::ERR_NETWORK, mod);
            // 删除网络回复对象，避免内存泄漏
            reply->deleteLater();
            return;
        }qDebug() << "你好";
        // 处理正确的情况：从网络回复中读取响应数据，并发送信号通知完成，传递响应数据、成功状态和模块信息
        QString res = reply->readAll();
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        // 删除网络回复对象，避免内存泄漏
        reply->deleteLater();
        return;
    });
}

void Httpmgr::slot_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        //发送信号通知指定模块http响应已结束
        emit sig_reg_mod_finish(req_id, res, err);
    }
    if(mod == Modules::RESETMOD){
        // 发送信号通知重置模块http响应已结束
        emit sig_reset_mod_finish(req_id, res, err);
    }
    if(mod == Modules::LOGINMOD){
        // 发送信号通知重置模块http响应已结束
        emit sig_login_mod_finish(req_id, res, err);
    }
    //...后续的模块操作
}
