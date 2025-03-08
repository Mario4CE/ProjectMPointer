#pragma once

#include "servidor.g.h"

namespace winrt::ProjectMPointer::implementation
{
    struct servidor : servidorT<servidor>
    {
        servidor() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::ProjectMPointer::factory_implementation
{
    struct servidor : servidorT<servidor, implementation::servidor>
    {
    };
}
