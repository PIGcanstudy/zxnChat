# proto文件编写

- 在项目的根目录下创建一个proto名字为message.proto

```cpp

syntax = "proto3";

package message;

service VarifyService {
  rpc GetVarifyCode (GetVarifyReq) returns (GetVarifyRsp) {}
}

message GetVarifyReq {
  string email = 1;
}

message GetVarifyRsp {
  int32 error = 1;
  string email = 2;
  string code = 3;
}

```

- 接下来我们利用grpc编译后生成的proc.exe生成proto的grpc的头文件和源文件

```cpp

 D:\grpc\C++grpc\grpc\visualpro\third_party\protobuf\Debug\protoc.exe  -I="." --grpc_out="." --plugin=protoc-gen-grpc="D:\grpc\C++grpc\grpc\visualpro\Debug\grpc_cpp_plugin.exe" "message.proto"

```

- 上述命令会生成message.grpc.pb.h和message.grpc.pb.cc文件。

- 接下来我们生成用于序列化和反序列化的pb文件

```cpp
D:\grpc\C++grpc\grpc\visualpro\third_party\protobuf\Debug\protoc.exe --cpp_out=. "message.proto"
```

- 这样就准备好了