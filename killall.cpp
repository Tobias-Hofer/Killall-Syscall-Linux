#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int string_contains(const char *haystack, const char *needle) {
    return strstr(haystack, needle) != NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return -1;
    }

    const char *target = argv[1];
    DIR *proc = opendir("/proc");
    if (!proc) {
        return -1;
    }

    struct dirent *directoryEntry;
    while ((directoryEntry = readdir(proc)) != NULL) {
        pid_t pid = atoi(directoryEntry->d_name);
        if (pid <= 0)
            continue;

        char filePath[128];
        snprintf(filePath, sizeof(filePath), "/proc/%d/comm", pid);

        int fileDescriptor = open(filePath, O_RDONLY);
        if (fileDescriptor < 0)
            continue;

        char processName[128];
        ssize_t bytesRead = read(fileDescriptor, processName, sizeof(processName) - 1);
        close(fileDescriptor);
        if (bytesRead <= 0)
            continue;

        processName[bytesRead] = '\0'; 
        char *newLine = strchr(processName, '\n');
        if (newLine)
            *newLine = '\0';

        if (string_contains(processName, target)) {
            kill(pid, SIGKILL);
        }
    }

    closedir(proc);
    return 0;
}

