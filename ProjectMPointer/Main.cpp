#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread] // Atributo necesario para aplicaciones de Windows Forms
int main(array<String^>^ args) {
    Application::EnableVisualStyles(); // Habilita estilos visuales
    Application::SetCompatibleTextRenderingDefault(false); // Configura el renderizado de texto

    // Crear una instancia del formulario principal y ejecutar la aplicación
    ProjectMPointer::MyForm form;
    Application::Run(% form);

    return 0;
}