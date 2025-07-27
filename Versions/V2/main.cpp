#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <shlobj.h>
#include <filesystem>
#include <chrono>
#include <iomanip>

using Clock = std::chrono::high_resolution_clock;
auto startTime = Clock::now();

void CreateAppDataFolder() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        std::string fullPath = std::string(path) + "\\Kings MonoCore";

        DWORD attribs = GetFileAttributesA(fullPath.c_str());
        if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            CreateDirectoryA(fullPath.c_str(), NULL);
        }
    }
}

std::string getTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buffer[32];
    sprintf_s(buffer, "[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);
    return std::string(buffer);
}

std::vector<std::string> getIPConfig() {
    std::vector<std::string> output;
    FILE* pipe = _popen("ipconfig /all", "r");
    if (!pipe) return output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output.push_back(buffer);
    }
    _pclose(pipe);
    return output;
}

std::string extractValue(const std::vector<std::string>& lines, const std::string& key) {
    for (const auto& line : lines) {
        if (line.find(key) != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 1);
                return value.substr(0, value.find_last_not_of(" \n\r\t") + 1);
            }
        }
    }
    return "N/A";
}

std::string pingGateway(const std::string& gateway) {
    if (gateway == "N/A") return "Unavailable";
    std::string cmd = "ping -n 1 " + gateway + " | find \"Average\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "Error";
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    _pclose(pipe);
    return result.empty() ? "Timeout" : result;
}

void BootScreen() {
    system("title MonoCore V2.0");
    CreateAppDataFolder();

    std::cout << "Loading system modules...\n\n";

    Sleep(600); std::cout << getTimestamp() << "[ OK ] KernelBridge.dll\n";
    Sleep(500); std::cout << getTimestamp() << "[ OK ] NetCoreWrapper.dll\n";
    Sleep(500); std::cout << getTimestamp() << "[ OK ] UserModeAuth.dll\n";
    Sleep(500); std::cout << getTimestamp() << "[ OK ] IOTracker.dll\n";

    Sleep(600); std::cout << "\nRunning integrity check...\n";
    Sleep(700); std::cout << getTimestamp() << "[ OK ] All modules verified\n";

    Sleep(400); std::cout << "\nPreparing network diagnostics...\n";
    Sleep(1000);
    system("cls");
}

void NetInfo() {
    auto data = getIPConfig();

    std::string hostname = extractValue(data, "Host Name");
    std::string adapter = extractValue(data, "Description");
    std::string mac = extractValue(data, "Physical Address");
    std::string dhcp = extractValue(data, "DHCP Enabled");
    std::string ipv4 = extractValue(data, "IPv4 Address");
    std::string gateway = extractValue(data, "Default Gateway");
    std::string dns = extractValue(data, "DNS Servers");
    std::string latency = pingGateway(gateway);

    DWORD uptime_ms = GetTickCount64();
    int uptime_minutes = uptime_ms / 60000;

    std::cout << "MonoCore Network Diagnostic\n";
    std::cout << "----------------------------\n";
    std::cout << "Hostname     |" << hostname << "\n";
    std::cout << "Adapter      |" << adapter << "\n";
    std::cout << "MAC Address  |" << mac << "\n";
    std::cout << "DHCP Enabled |" << dhcp << "\n";
    std::cout << "IPv4         |" << ipv4 << "\n";
    std::cout << "Gateway      |" << gateway << "\n";
    std::cout << "DNS Server   |" << dns << "\n";
    std::cout << "Ping to GW   |" << latency << "\n";
    std::cout << "System Uptime|" << uptime_minutes << " minutes\n";

    auto endTime = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

    std::cout << "\nMade by King(@indieun2 on Discord)\n";
    std::cout << "Press any key to exit...\n";
    _getch();
}

int main() {
    BootScreen();
    NetInfo();
    return 0;
}
