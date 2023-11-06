#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <gflags/gflags.h>
#include "hello.grpc.pb.h"

DEFINE_string(address,
	"127.0.0.1",
	"ip address"
	);

DEFINE_int32(
	port,
	50051,
	"port"
);

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hello::HelloRequest;
using hello::HelloReply;

using hello::HelloWorld;

class HelloWorldServiceImpl final : public HelloWorld::Service
{
	virtual ::grpc::Status SayHelloWorld(::grpc::ServerContext* context, const ::hello::EmptyRequest* request, ::hello::HelloReply* response)
	{
		response->set_message("hello,world");
		return Status::OK;

	}
	virtual ::grpc::Status HelloFromServer(::grpc::ServerContext* context, const ::hello::HelloRequest* request, ::hello::HelloReply* response)
	{
		std::string response_message = "Hello, " + request->name();
		response->set_message(response_message);
		return Status::OK;
	}
};

void RunServer(const std::string& address, int port)
{
	std::string server_address = address + ":" + std::to_string(port);
	HelloWorldServiceImpl service;
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on: " << server_address;
	server->Wait();
}

int main(int argc, char** argv)
{
	gflags::SetUsageMessage("This is a sample program for parsing IP address and port.");
	gflags::SetVersionString("0.0.1");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	RunServer(FLAGS_address, FLAGS_port);
}