#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <gflags/gflags.h>
#include <vector>
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

	virtual ::grpc::Status ReceiveObject(::grpc::ServerContext* context, const ::hello::Object* request, ::hello::EmptyReply* response)
	{
		const std::string& vertices_data = request->vertices();
		size_t num_floats = vertices_data.size() / sizeof(float);
		std::vector<float> float_vector(num_floats);
		std::memcpy(float_vector.data(), vertices_data.data(), vertices_data.size());
		const std::string& indices_data = request->indices();
		size_t num_indices = indices_data.size() / sizeof(int64_t);
		std::vector<int64_t> indices_vector(num_indices);
		std::memcpy(indices_vector.data(), indices_data.data(), indices_data.size());
		const std::string& normals_data = request->normals();
		size_t num_normals = normals_data.size() / sizeof(float);
		std::vector<float> normal_vector(num_normals);
		std::memcpy(normal_vector.data(), normals_data.data(), normals_data.size());
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