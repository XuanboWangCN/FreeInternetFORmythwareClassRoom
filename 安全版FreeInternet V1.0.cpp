#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <tlhelp32.h>
#include <shellapi.h>

using namespace std;

// 函数声明
bool IsRunAsAdmin();
void ElevateSelf();
void PrintTitle();
bool KillProcess(const wstring& processName);
bool StopService(const wchar_t* serviceName);
void RunHiddenCommand(const wstring& command);

int main() {
    // 设置控制台属性
    SetConsoleOutputCP(936);
    SetConsoleTitleW(L"Windows 文件资源管理器");
    system("mode con cols=60 lines=20");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 0xFF);
    system("cls");

    PrintTitle();

    // 检查管理员权限
    if (!IsRunAsAdmin()) {
        cout << "[提示]: 本操作需要管理员权限！\n\n";
        cout << "正在请求管理员权限,如果弹出窗口请单击 是\n";
        ElevateSelf();
        return 0;
    }



    // 结束进程
    if (KillProcess(L"MasterHelper.exe")) {
        cout << "OK\n";
    } else {
        cout << "[警告]失败\n";
    }

    // 停止服务
    cout << "OK\n";
    if (StopService(L"tdnetfilter")) {
        cout << "OK\n";
    } else {
        cout << "[警告]失败\n";
    }

    // 完成信息
    system("powershell -Command \"Add-Type -AssemblyName PresentationFramework; [System.Windows.MessageBox]::Show('OK', 'OK', 'OK', 'Information')\"");
    cout << "\n";
    

    // 打开网站
    ShellExecuteW(NULL, L"open", L"https://xuanbo.top/tools/networktesting", NULL, NULL, SW_SHOW);
    return 0;
}

bool IsRunAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY auth = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&auth, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    } 
    return isAdmin;
}

void ElevateSelf() {
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = path;
    sei.nShow = SW_NORMAL;
    
    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        if (err == ERROR_CANCELLED) {
            cout << "操作已取消\n";
        } else {
            cout << "提升权限失败 (" << err << ")\n";
        }
        exit(1);
    }
    exit(0);
}

void PrintTitle() {
    cout << "启动中 by bob_cn@qq.com\n";
}

bool KillProcess(const wstring& processName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe = { sizeof(pe) };
    bool found = false;

    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProc) {
                    TerminateProcess(hProc, 0);
                    CloseHandle(hProc);
                    found = true;
                }
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return found;
}

bool StopService(const wchar_t* serviceName) {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) return false;

    SC_HANDLE service = OpenServiceW(scm, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!service) {
        CloseServiceHandle(scm);
        return false;
    }

    SERVICE_STATUS status;
    bool success = ControlService(service, SERVICE_CONTROL_STOP, &status);
    
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return success;
}
