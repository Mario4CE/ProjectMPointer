#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;

int main(int arg, char* argv[]) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    ProjectMPointer::MyForm form;
    Application::Run(% form);
    RunServer();
    return 0;
}