#pragma once

namespace InterfazCLI {

    public ref class FormularioPrincipal : public System::Windows::Forms::Form
    {
    private: System::Windows::Forms::Label^ lblTitulo;
    public:
        FormularioPrincipal()
        {
            InitializeComponent();
        }

    private:
        void InitializeComponent()
        {
            this->lblTitulo = (gcnew System::Windows::Forms::Label());
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
            // FormularioPrincipal
            // 
            this->ClientSize = System::Drawing::Size(670, 346);
            this->Controls->Add(this->lblTitulo);
            this->Name = L"FormularioPrincipal";
            this->Text = L"Servidor";
            this->TransparencyKey = System::Drawing::Color::Red;
            this->ResumeLayout(false);
            this->PerformLayout();

        }

    };
}