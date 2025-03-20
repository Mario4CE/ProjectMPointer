#include <Windows.h>
#pragma once

namespace InterfazCLI {

    public ref class FormularioPrincipal : public System::Windows::Forms::Form
    {
    private: System::Windows::Forms::Label^ lblRespuesta;

    private: System::Windows::Forms::Label^ lblTitulo;
    public:
        FormularioPrincipal()
        {
            InitializeComponent();
        }

        // Método público para actualizar el texto de lblRespuesta
        void ActualizarRespuesta(System::String^ texto) {
            if (this->lblRespuesta->InvokeRequired) { // Verifica si se necesita invocar
                this->lblRespuesta->Invoke(gcnew System::Action<System::String^>(this, &FormularioPrincipal::ActualizarRespuesta), texto);
            }
            else {
                this->lblRespuesta->Text = texto;
            }
        }

    private:
        void InitializeComponent()
        {
            this->lblTitulo = (gcnew System::Windows::Forms::Label());
            this->lblRespuesta = (gcnew System::Windows::Forms::Label());
            this->SuspendLayout();
            // 
            // lblTitulo
            // 
            this->lblTitulo->AutoSize = true;
            this->lblTitulo->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
            this->lblTitulo->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 21.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblTitulo->Location = System::Drawing::Point(214, 19);
            this->lblTitulo->Name = L"lblTitulo";
            this->lblTitulo->Size = System::Drawing::Size(211, 33);
            this->lblTitulo->TabIndex = 0;
            this->lblTitulo->Text = L"Server MPointers";
            // 
            // lblRespuesta
            // 
            this->lblRespuesta->BackColor = System::Drawing::Color::LightGray;
            this->lblRespuesta->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblRespuesta->Location = System::Drawing::Point(12, 72);
            this->lblRespuesta->Name = L"lblRespuesta";
            this->lblRespuesta->Size = System::Drawing::Size(646, 248);
            this->lblRespuesta->TabIndex = 3;
            this->lblRespuesta->Text = L"Respuesta del servidor aparecerá aquí.";
            this->lblRespuesta->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            // 
            // FormularioPrincipal
            // 
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