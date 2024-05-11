#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QStyle>
#include <QRegularExpression>
//QT的网络，需要在qmake或者cmake中加network
//网络管理
#include <QNetworkAccessManager>
//QT的json序列化
#include <QJsonObject>
//QT的json反序列化
#include <QJsonDocument>

#include <QSettings>
/**
 * @brief repolish 用来刷新qss的
 */
extern std::function<void(QWidget*)> repolish;

/**
 * @brief The ReqId enum 请求id
 */
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, // 用户注册
};

/**
 * @brief The Modules enum 模块的id
 */
enum Modules{
    REGISTERMOD = 0, //注册模块
};

/**
 * @brief The ErrorCodes enum 错误码
 */

enum ErrorCodes{
    SUCCESS = 0, //成功
    ERR_JSON = 1, //json解析失败
    ERR_NETWORK = 2,//网络错误
};

extern QString gate_url_prefix;
#endif // GLOBAL_H
