#include "MyForm.h"
#include <iostream>
#include <memory>
#include <string>

Status MiServicioImpl::Saludar(ServerContext* context, const SaludoRequest* request, SaludoResponse* reply) {
    std::string nombre = request->nombre();
    std::string mensaje = "Hola, " + nombre + "!";
    reply->set_mensaje(mensaje);
    return Status::OK;
}

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    MiServicioImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Servidor escuchando en " << server_address << std::endl;
    server->Wait();
}