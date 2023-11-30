#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <dirent.h>

std::string buildDirectoryHtmlElement(std::string dirName, std::string path)
{
    return ("<li><a href=" + path + "/" + dirName + ">" + dirName + "</a></li>");
}

void listDirectoryContents(const std::string& directoryPath) {
    DIR* dir = opendir(directoryPath.c_str());
    std::string htmlBody1("<!DOCTYPE html> <html> <head> <title>List of Directories</title> </head> <body> <h1>List of Directories</h1> <ul>");
    std::string dirList("");
    std::string htmlBody2("</ul> </body> </html>");

    if (!dir) {
        std::cerr << "Failed to open directory: " << directoryPath << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        // Filter out "." entries
        if (std::strcmp(entry->d_name, ".") == 0) {
            continue;
        }        
        dirList += buildDirectoryHtmlElement(entry->d_name, directoryPath);
    }
    std::cout << htmlBody1 << dirList << htmlBody2;
    closedir(dir);
}

int main() {
    std::string directoryPath = "/home/lwidmer/git/git_webserv";
    listDirectoryContents(directoryPath);
    return 0;
}