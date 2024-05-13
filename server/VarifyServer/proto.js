const path = require("path");
const grpc = require("@grpc/grpc-js");
const protoLoader = require("@grpc/proto-loader");

const PROTO_PATH = path.join(__dirname, "message.proto");

try {
    const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
        keepCase: true,
        longs: String,
        enums: String,
        defaults: true,
        oneofs: true
    });

    const protoDescriptor = grpc.loadPackageDefinition(packageDefinition);
    
    console.log(protoDescriptor);
    const message_proto = protoDescriptor.message;

    console.log(message_proto);

    module.exports = message_proto;
} catch (error) {
    console.error("Error loading proto file:", error);
}
