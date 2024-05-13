/*
这是用来启动 grpc server的
*/

//包含头文件
const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid');
const emailModule = require('./email');
//const redis_module = require('./redis')


//GetVarifyCode声明为async是为了能在内部调用await。
async function GetVarifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        //生成验证码
        uniqueId = uuidv4();
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

        //对应message.proto 的GetVarifyRsp
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success
        }); 
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
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('grpc server started')        
    })
}

main()