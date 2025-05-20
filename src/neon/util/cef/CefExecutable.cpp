//
// Created by gaeqs on 20/05/25.
//

#include <cef_app.h>
#include <iostream>

int main(int argc, char** argv)
{
#if defined(_WIN32)
    // En Windows CefMainArgs espera un HINSTANCE:
    CefMainArgs main_args(::GetModuleHandle(nullptr));
#else
    // En Linux/macOS “normal”:
    CefMainArgs main_args(argc, argv);
#endif
    std::cout << "Running subprocess..." << std::endl;
    return CefExecuteProcess(main_args, nullptr, nullptr);
}