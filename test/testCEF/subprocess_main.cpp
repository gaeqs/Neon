//
// Created by gaeqs on 18/05/25.
//

int main(int argc, char* argv[]) {
    CefMainArgs main_args(argc, argv);
    // No hace initialize ni CreateBrowser; solo ejecuta sub-procesos.
    return CefExecuteProcess(main_args, /*app*/ nullptr, /*windows_sandbox_info*/ nullptr);
}