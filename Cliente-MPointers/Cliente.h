#pragma once

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
	public ref class Cliente : public System::Windows::Forms::Form
	{
	public:
		Cliente(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Cliente()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ btnCliente;
	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btnCliente = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// btnCliente
			// 
			this->btnCliente->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnCliente->Location = System::Drawing::Point(262, 12);
			this->btnCliente->Name = L"btnCliente";
			this->btnCliente->Size = System::Drawing::Size(224, 80);
			this->btnCliente->TabIndex = 0;
			this->btnCliente->Text = L"Inciar Servidor";
			this->btnCliente->UseVisualStyleBackColor = true;
			// 
			// Cliente
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(771, 374);
			this->Controls->Add(this->btnCliente);
			this->Name = L"Cliente";
			this->Text = L"Cliente";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
