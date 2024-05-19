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
 * @brief xorString 用来加密密码的
 * 这是为了让密码被泄露了也不会透露用户隐私用的
 */

extern std::function<QString(QString)> xorString;

/**
 * @brief The ReqId enum 请求id
 */
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, // 用户注册
    ID_RESET_PWD = 1003, //重置密码
};

/**
 * @brief The Modules enum 模块的id
 */
enum Modules{
    REGISTERMOD = 0, //注册模块
    RESETMOD = 1, //重置密码模块
    LOGINMOD = 2, // 登录模块
};

/**
 * @brief The ErrorCodes enum 错误码
 */

enum ErrorCodes{
    SUCCESS = 0, //成功
    ERR_JSON = 1, //json解析失败
    ERR_NETWORK = 2,//网络错误
};

/**
 * @brief The TipErr enum 错误输出框输出内容的key值
 */
enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

/**
 * @brief The ClickLbState enum 对应可见与不可见的状态
 */
enum ClickLbState{
    Normal = 0,
    Selected = 1
};

extern QString gate_url_prefix;
#endif // GLOBAL_H
