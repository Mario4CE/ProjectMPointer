#pragma once
#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <grpcpp/grpcpp.h>
#include "mi_servicio.grpc.pb.h"

namespace ProjectMPointer {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using grpc::Server;
    using grpc::ServerBuilder;
    using grpc::ServerContext;
    using grpc::Status;
    using ejemplo::MiServicio;
    using ejemplo::SaludoRequest;
    using ejemplo::SaludoResponse;

    /// <summary>
    /// Clase que implementa el servidor gRPC.
    /// </summary>
    class MiServicioImpl final : public MiServicio::Service {
    public:
        Status Saludar(ServerContext* context, const SaludoRequest* request, SaludoResponse* reply) override {
            std::string nombre = request->nombre();
            std::string mensaje = "Hola, " + nombre + "!";
            reply->set_mensaje(mensaje);
            return Status::OK;
        }
    };

    /// <summary>
    /// Función para ejecutar el servidor gRPC en segundo plano.
    /// </summary>
    void RunServer() {
        std::string server_address("0.0.0.0:50051");
        MiServicioImpl service;

        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        std::unique_ptr<Server> server(builder.BuildAndStart());
        System::Windows::Forms::MessageBox::Show("Servidor gRPC iniciado en " + gcnew System::String(server_address.c_str()));
        server->Wait();
    }

    /// <summary>
    /// Summary for MyForm
    /// </summary>
    public ref class MyForm : public System::Windows::Forms::Form {
    public:
        MyForm(void) {
            InitializeComponent();
            // Iniciar el servidor gRPC en un hilo separado
            System::Threading::Thread^ serverThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(&RunServer));
            serverThread->Start();
        }

    protected:
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~MyForm() {
            if (components) {
                delete components;
            }
        }

    private:
        System::Windows::Forms::Label^ lbltitulo;
        System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void) {
            this->lbltitulo = (gcnew System::Windows::Forms::Label());
            this->SuspendLayout();
            // 
            // lbltitulo
            // 
            this->lbltitulo->AutoSize = true;
            this->lbltitulo->Font = (gcnew System::Drawing::Font(L"Arial Black", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lbltitulo->ForeColor = System::Drawing::SystemColors::ActiveCaption;
            this->lbltitulo->Location = System::Drawing::Point(350, 28);
            this->lbltitulo->Name = L"lbltitulo";
            this->lbltitulo->Size = System::Drawing::Size(174, 45);
            this->lbltitulo->TabIndex = 0;
            this->lbltitulo->Text = L"MPointer";
            this->lbltitulo->Click += gcnew System::EventHandler(this, &MyForm::label1_Click);
            // 
            // MyForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(885, 430);
            this->Controls->Add(this->lbltitulo);
            this->Name = L"MyForm";
            this->Text = L"Memory Manager";
            this->ResumeLayout(false);
            this->PerformLayout();
        }
#pragma endregion

    private:
        System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
            // Manejar el evento de clic en el label si es necesario
        }
    };
}

#endif // SERVIDOR_H