//引入文件系统
const fs = require('fs');

//用utf-8的方式来读取内容并解析
let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
let email_user = config.email.user;
let email_pass = config.email.pass;
let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;
//redis需要用到此前缀
let code_prefix = "code_";

//将之前定义的所有变量作为一个对象，导出为模块的默认输出
//他文件可以通过 require 函数引入这个模块，并获取其中定义的变量。
module.exports = {email_pass, email_user, mysql_host, mysql_port,redis_host, redis_port, redis_passwd, code_prefix}