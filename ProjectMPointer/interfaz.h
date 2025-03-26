
#pragma once

#pragma once
#include <Windows.h>
#include <msclr/marshal_cppstd.h>
#include <string>
#include <iostream>

namespace InterfazCLI {

    public ref class FormularioPrincipal : public System::Windows::Forms::Form
    {
    private:
        System::Windows::Forms::Label^ lblRespuesta;
        System::Windows::Forms::Label^ lblTitulo;
        System::Net::Sockets::TcpClient^ client;
        System::Net::Sockets::NetworkStream^ stream;
        System::ComponentModel::BackgroundWorker^ backgroundWorker;

    public:
        FormularioPrincipal()
        {
            InitializeComponent();
            InitializeNetwork();
        }

        // Método público para actualizar el texto de lblRespuesta
        void ActualizarRespuesta(System::String^ texto) {
            if (this->lblRespuesta->InvokeRequired) {
                this->lblRespuesta->Invoke(gcnew System::Action<System::String^>(this, &FormularioPrincipal::ActualizarRespuesta), texto);
            }
            else {
                this->lblRespuesta->Text = texto;
            }
        }

        // Método para enviar mensajes al servidor
        void SendMessage(System::String^ mensaje) {
            if (this->client != nullptr && this->client->Connected) {
                array<System::Byte>^ data = System::Text::Encoding::ASCII->GetBytes(mensaje + "\n");

                try {
                    this->stream->Write(data, 0, data->Length);
                    this->stream->Flush();
                }
                catch (System::Exception^ e) {
                    this->lblRespuesta->Text = "Error enviando mensaje: " + e->Message;
                    Reconnect();
                }
            }
            else {
                Reconnect();
            }
        }

        void Reconnect() {
            try {
                if (this->client != nullptr) {
                    this->client->Close();
                }
                InitializeNetwork();
            }
            catch (System::Exception^ e) {
                this->lblRespuesta->Text = "Error de reconexión: " + e->Message;
            }
        }

    private:
        void InitializeNetwork() {
            this->client = gcnew System::Net::Sockets::TcpClient();
            try {
                this->client->Connect("127.0.0.1", 12345);
                this->stream = client->GetStream();

                // Configurar BackgroundWorker para recibir mensajes
                this->backgroundWorker = gcnew System::ComponentModel::BackgroundWorker();
                this->backgroundWorker->WorkerReportsProgress = true;
                this->backgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &FormularioPrincipal::BackgroundWorker_DoWork);
                this->backgroundWorker->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &FormularioPrincipal::BackgroundWorker_ProgressChanged);
                this->backgroundWorker->RunWorkerAsync();

            }
            catch (System::Exception^ e) {
                this->lblRespuesta->Text = "Error de conexión: " + e->Message;
            }
        }

        void BackgroundWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
            array<System::Byte>^ buffer = gcnew array<System::Byte>(1024);

            while (this->client->Connected) {
                try {
                    int bytesRead = this->stream->Read(buffer, 0, buffer->Length);
                    if (bytesRead > 0) {
                        System::String^ response = System::Text::Encoding::ASCII->GetString(buffer, 0, bytesRead);
                        this->backgroundWorker->ReportProgress(0, response);
                    }
                }
                catch (System::Exception^) {
                    this->backgroundWorker->ReportProgress(1, "Error de conexión");
                    break;
                }
            }
        }

        void BackgroundWorker_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e) {
            if (e->ProgressPercentage == 0) {
                this->lblRespuesta->Text = dynamic_cast<System::String^>(e->UserState);
            }
            else {
                this->lblRespuesta->Text = dynamic_cast<System::String^>(e->UserState);
                Reconnect();
            }
        }

        void InitializeComponent()
        {
            this->lblTitulo = (gcnew System::Windows::Forms::Label());
            this->lblRespuesta = (gcnew System::Windows::Forms::Label());
            this->SuspendLayout();

            // lblTitulo
            this->lblTitulo->AutoSize = true;
            this->lblTitulo->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
            this->lblTitulo->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 21.75F, System::Drawing::FontStyle::Bold));
            this->lblTitulo->Location = System::Drawing::Point(214, 19);
            this->lblTitulo->Name = L"lblTitulo";
            this->lblTitulo->Size = System::Drawing::Size(211, 33);
            this->lblTitulo->TabIndex = 0;
            this->lblTitulo->Text = L"Server MPointers";

            // lblRespuesta
            this->lblRespuesta->BackColor = System::Drawing::Color::LightGray;
            this->lblRespuesta->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 27.75F));
            this->lblRespuesta->Location = System::Drawing::Point(12, 72);
            this->lblRespuesta->Name = L"lblRespuesta";
            this->lblRespuesta->Size = System::Drawing::Size(646, 248);
            this->lblRespuesta->TabIndex = 3;
            this->lblRespuesta->Text = L"Respuesta del servidor aparecerá aquí.";
            this->lblRespuesta->TextAlign = System::Drawing::ContentAlignment::TopCenter;

            // FormularioPrincipal
            this->ClientSize = System::Drawing::Size(670, 346);
            this->Controls->Add(this->lblRespuesta);
            this->Controls->Add(this->lblTitulo);
            this->Name = L"FormularioPrincipal";
            this->Text = L"Servidor";
            this->TransparencyKey = System::Drawing::Color::Red;
            this->ResumeLayout(false);
            this->PerformLayout();
        }
    };
}