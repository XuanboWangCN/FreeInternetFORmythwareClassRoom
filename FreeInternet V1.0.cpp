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
    SetConsoleTitleW(L"「Hack Class Network」V1.0 by bob_cn@qq.com");
    system("mode con cols=60 lines=20");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 0xF2);
    system("cls");

    PrintTitle();

    // 检查管理员权限
    if (!IsRunAsAdmin()) {
        cout << "[提示]: 本操作需要管理员权限！\n\n";
        cout << "正在请求管理员权限,如果弹出窗口请单击 是\n";
        ElevateSelf();
        return 0;
    }

    // 用户提示
    cout << "[提示]:\n";
    cout << "本工具仅用于解除 极域电子教室 中的软件级别的网络访问限制，仅在 极域电子教室2016 版本中测试过\n";
    cout << "使用者需合法合规使用\n";
    cout << "认真遵守课堂纪律，不得干扰教学秩序。产生的所有后果请自行承担，软件开发者不承担任何责任\n\n";
    Sleep(1000);
    cout << "–—–—–—–—–—–—\n按下键盘上的任意按键以继续";
    _getch();

    // 清屏保留标题
    system("cls");
    PrintTitle();
    Sleep(1000);

    // 结束进程
    cout << "[系统]: 正在结束 MasterHelper.exe 进程...\n";
    if (KillProcess(L"MasterHelper.exe")) {
        cout << "[系统]: MasterHelper.exe 进程已结束！\n";
    } else {
        cout << "[警告]: 未找到 MasterHelper.exe 进程或结束失败，可能是因为已经解除了限制或未被限制，请重新尝试\n";
    }
    Sleep(1000);

    // 停止服务
    cout << "[系统]: 正在停止 tdnetfilter 服务...\n";
    if (StopService(L"tdnetfilter")) {
        cout << "[系统]: 服务已停止！\n";
    } else {
        cout << "[警告]: tdnetfilter 服务停止失败，可能是因为已经解除了限制或未被限制，请重新尝试\n";
    }
    Sleep(1000);

    // 完成信息
    cout << "[系统]: 操作完成，可以尝试访问互联网\n\n";
    cout << "–—–—–—–—–—–—\n";
    cout << "按下键盘上的任意按键以退出程序";
    _getch();

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
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "  电子极域教室解除网络限制工具 V1.0\n";
    cout << "        作者E-mail:bob_cn@qq.com\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
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
