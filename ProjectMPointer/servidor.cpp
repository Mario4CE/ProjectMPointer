#include "pch.h"
#include "servidor.h"
#if __has_include("servidor.g.cpp")
#include "servidor.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::ProjectMPointer::implementation
{
    int32_t servidor::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void servidor::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void servidor::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
