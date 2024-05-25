#ifndef TCPMGR_H
#define TCPMGR_H
#include <QTcpSocket>
#include "Singleton.h"
#include "global.h"
#include <functional>
#include <QObject>
class TcpMgr: public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    friend class Singleton<TcpMgr>;
    ~TcpMgr();
private:
    TcpMgr();
    void initHandler();
    void handleMsg(ReqId id, uint16_t len, QByteArray data);
    // TCP
    QTcpSocket _socket;
    // ip地址
    QString _host;
    // 端口
    uint16_t _port;
    // 用来标记数据是否发送完成
    bool _b_recv_pending;
    // 消息id
    uint16_t _message_id;
    // 消息长度
    uint16_t _message_len;
    // 接收缓冲区
    QByteArray _buffer;
    // 处理逻辑和id统一起来
    QMap<ReqId, std::function<void(ReqId id, uint16_t len, QByteArray data)>> _handlers;
signals:
    // tcp连接成功的信号
    void sig_con_success(bool bsuccess);
    // 可以发送数据的信号
    void sig_send_data(ReqId reqId, QString data);
    // 转变到聊天界面的槽函数
    void sig_switch_chatdlg();
    // 登录失败的槽函数
    void sig_login_failed(int);

public slots:
    // 用来连接tcp的槽函数
    void slot_tcp_connect(ServerInfo si);
    // 可发送数据的槽函数
    void slot_send_data(ReqId reqId, QString data);
};

#endif // TCPMGR_H
