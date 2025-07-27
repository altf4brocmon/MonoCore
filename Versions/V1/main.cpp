#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <shlobj.h>  
#include <filesystem>

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

void BootScreen() {
    system("title MonoCore V1.0");

    std::cout << "Loading system modules...\n\n";

    Sleep(600); std::cout << "[ OK ] KernelBridge.dll\n";
    Sleep(500); std::cout << "[ OK ] NetCoreWrapper.dll\n";
    Sleep(500); std::cout << "[ OK ] UserModeAuth.dll\n";
    Sleep(500); std::cout << "[ OK ] IOTracker.dll\n";

    Sleep(600); std::cout << "\nRunning integrity check...\n";
    Sleep(700); std::cout << "[ OK ] All modules verified\n";

    Sleep(400); std::cout << "\nPreparing network diagnostics...\n";
    Sleep(1000);

    system("cls");
}

void NetInfo() {
    auto data = getIPConfig();

    std::string ipv4 = extractValue(data, "IPv4 Address");
    std::string gateway = extractValue(data, "Default Gateway");
    std::string dns = extractValue(data, "DNS Servers");
    std::string adapter = extractValue(data, "Description");
    std::string dhcp = extractValue(data, "DHCP Enabled");
    std::string hostname = extractValue(data, "Host Name");

    std::cout << "MonoCore Network Diagnostic\n";
    std::cout << "----------------------------\n";
    std::cout << "Hostname   |" << hostname << "\n";
    std::cout << "Adapter    |" << adapter << "\n";
    std::cout << "DHCP       |" << dhcp << "\n";
    std::cout << "IPv4       |" << ipv4 << "\n";
    std::cout << "Gateway    |" << gateway << "\n";
    std::cout << "DNS        |" << dns << "\n";

    std::cout << "\nPress any key to exit...\n";
    _getch();
}

int main() {
    BootScreen();
    NetInfo();
    return 0;
}
