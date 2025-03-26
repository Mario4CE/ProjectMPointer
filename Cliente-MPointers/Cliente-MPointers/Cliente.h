
#pragma once

#include <msclr/gcroot.h>
#include "MPointer.h"

namespace ClienteMPointers {
    using namespace System;
    using namespace System::Windows::Forms;

    public ref class Cliente : public Form {
    private:
        MPointer<int>* mptrInt;
        MPointer<double>* mptrDouble;
        MPointer<float>* mptrFloat;
        MPointer<std::string>* mptrStr;

    public:
        Cliente(void) {
            InitializeComponent();

            // Inicialización correcta
            mptrInt = new MPointer<int>();
            mptrDouble = new MPointer<double>();
            mptrFloat = new MPointer<float>();
            mptrStr = new MPointer<std::string>();

            // Configuración del servidor
            MPointer<int>::Init("127.0.0.1", 12345);
            MPointer<double>::Init("127.0.0.1", 12345);
            MPointer<float>::Init("127.0.0.1", 12345);
            MPointer<std::string>::Init("127.0.0.1", 12345);

        }

    protected:
        ~Cliente() {
            if (mptrInt != nullptr) {
                delete mptrInt;
                mptrInt = nullptr;
            }
            if (mptrDouble != nullptr) {
                delete mptrDouble;
                mptrDouble = nullptr;
            }
            if (mptrFloat != nullptr) {
                delete mptrFloat;
                mptrFloat = nullptr;
            }
            if (mptrStr != nullptr) {
                delete mptrStr;
                mptrStr = nullptr;
            }

        }

    private:
        // Controles de la interfaz gráfica
        System::Windows::Forms::Button^ btnCliente;
        System::Windows::Forms::TextBox^ txtPeticion;
        System::Windows::Forms::Label^ lblRespuesta;
        System::Windows::Forms::Label^ lblTitle;

        // Contenedor de componentes
        System::ComponentModel::Container^ components;

        // Manejador de eventos para el clic del botón
        System::Void btnCliente_Click(System::Object^ sender, System::EventArgs^ e);

        // Declaración de la función EnviarPeticion
        //std::string EnviarPeticion(const std::string& peticion); // Declaración correcta

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void) {
            this->btnCliente = (gcnew System::Windows::Forms::Button());
            this->txtPeticion = (gcnew System::Windows::Forms::TextBox());
            this->lblRespuesta = (gcnew System::Windows::Forms::Label());
            this->lblTitle = (gcnew System::Windows::Forms::Label());
            this->SuspendLayout();
            // 
            // btnCliente
            // 
            this->btnCliente->BackColor = System::Drawing::SystemColors::InactiveCaption;
            this->btnCliente->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->btnCliente->Location = System::Drawing::Point(434, 151);
            this->btnCliente->Name = L"btnCliente";
            this->btnCliente->Size = System::Drawing::Size(224, 80);
            this->btnCliente->TabIndex = 0;
            this->btnCliente->Text = L"Enviar Petición";
            this->btnCliente->UseVisualStyleBackColor = false;
            this->btnCliente->Click += gcnew System::EventHandler(this, &Cliente::btnCliente_Click);
            // 
            // txtPeticion
            // 
            this->txtPeticion->Font = (gcnew System::Drawing::Font(L"Arial", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->txtPeticion->Location = System::Drawing::Point(44, 97);
            this->txtPeticion->Name = L"txtPeticion";
            this->txtPeticion->Size = System::Drawing::Size(290, 50);
            this->txtPeticion->TabIndex = 1;
            // 
            // lblRespuesta
            // 
            this->lblRespuesta->BackColor = System::Drawing::Color::LightGray;
            this->lblRespuesta->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblRespuesta->Location = System::Drawing::Point(38, 172);
            this->lblRespuesta->Name = L"lblRespuesta";
            this->lblRespuesta->Size = System::Drawing::Size(308, 164);
            this->lblRespuesta->TabIndex = 2;
            this->lblRespuesta->Text = L"Respuesta del servidor aparecerá aquí.";
            this->lblRespuesta->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            // 
            // lblTitle
            // 
            this->lblTitle->AutoSize = true;
            this->lblTitle->BackColor = System::Drawing::SystemColors::InactiveCaption;
            this->lblTitle->Font = (gcnew System::Drawing::Font(L"Arial", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblTitle->Location = System::Drawing::Point(199, 20);
            this->lblTitle->Name = L"lblTitle";
            this->lblTitle->Size = System::Drawing::Size(379, 42);
            this->lblTitle->TabIndex = 3;
            this->lblTitle->Text = L"Servidor de MPointers";
            // 
            // Cliente
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(771, 374);
            this->Controls->Add(this->lblTitle);
            this->Controls->Add(this->btnCliente);
            this->Controls->Add(this->txtPeticion);
            this->Controls->Add(this->lblRespuesta);
            this->Name = L"Cliente";
            this->Text = L"Cliente";
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    };
}