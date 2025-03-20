#pragma once

#include <string>

namespace ClienteMPointers {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    /// <summary>
    /// Summary for Cliente
    /// </summary>
    public ref class Cliente : public System::Windows::Forms::Form {
    public:
        Cliente(void) {
            InitializeComponent();
        }

    protected:
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~Cliente() {
            if (components) {
                delete components;
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
        std::string EnviarPeticion(const std::string& peticion); // Declaración correcta

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
            this->btnCliente->Location = System::Drawing::Point(451, 129);
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
            this->txtPeticion->Location = System::Drawing::Point(56, 86);
            this->txtPeticion->Name = L"txtPeticion";
            this->txtPeticion->Size = System::Drawing::Size(290, 50);
            this->txtPeticion->TabIndex = 1;
            // 
            // lblRespuesta
            // 
            this->lblRespuesta->BackColor = System::Drawing::Color::LightGray;
            this->lblRespuesta->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblRespuesta->Location = System::Drawing::Point(50, 150);
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