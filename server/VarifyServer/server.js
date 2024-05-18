/*
这是用来启动 grpc server的
*/

//包含头文件
const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid');
const emailModule = require('./email');
const redis_module = require('./redis')


//GetVarifyCode声明为async是为了能在内部调用await。
async function GetVarifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        // const_module.code_prefix 表示验证码的前缀
        // 也就是 code_, 这是从数据库中通过key得到验证码value
        let query_res = await redis_module.GetRedis(const_module.code_prefix + call.request.email);
        console.log("query_res is ", query_res)
        // 表示之前没有生成过验证码
        let uniqueId = query_res;
        if(query_res == null){
            // 生成验证码
            uniqueId = uuidv4();
            if(uniqueId.length > 4){
                uniqueId = uniqueId.substring(0, 4);
            }
            // 设置邮箱，验证码以及过期时间时间到redis里
            // 邮箱前面会有code_, 比如 code_abc.qq.com
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix+call.request.email, uniqueId, 180)
            if(!bres){
                // 失败返回
                callback(null, { email:  call.request.email,
                    error:const_module.Errors.RedisErr
                });
                return;
            }
        }
        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为'+ uniqueId +'请三分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: '1464451627@qq.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };
        //用await来等待发送邮件的完成
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)
        if (send_res) {
            //对应message.proto 的GetVarifyRsp
            callback(null, {
                email: call.request.email,
                error: const_module.Errors.Success
            });
        } else {
            callback(null, {
                email: call.request.email,
                error: const_module.Errors.EmailSendFailure
            });
        }
    }catch(error){
        //表示邮箱发送失败了，对应email.js的reject
        console.log("catch error is ", error)
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }
}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VarifyService.service, { GetVarifyCode: GetVarifyCode })
    //第二个参数表示安全校验
    server.bindAsync('0.0.0.0:50053', grpc.ServerCredentials.createInsecure(), () => {
        console.log('grpc server started')        
    })
}

main()