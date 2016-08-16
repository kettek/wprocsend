#include "stdafx.h"
#include "Windows.h"
#include "TlHelp32.h" // process-related

char desc_string[] = "wprocsend, a utility for sending a signal to a process.\n";
char usage_string[] = "  Usage: wprocsend (int|break|kill) (pid|program.exe)\n";

char sig_strings[][6] = {
  "int",
  "break",
  "kill"
};

int findPID(const char *exe_name) {
  int pid = NULL;
  HANDLE process_snapshot;
  PROCESSENTRY32 pe32;

  const size_t WCHARBUF = 1000;
  wchar_t wname[WCHARBUF];

  // widen our string
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, exe_name, -1, wname, WCHARBUF);

  if ((process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) return 0;

  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(process_snapshot, &pe32)) {
    CloseHandle(process_snapshot);
    return 0;
  }

  do {
    if (wcscmp(wname, pe32.szExeFile) == 0) {
      pid = pe32.th32ProcessID;
      break;
    }
  } while (Process32Next(process_snapshot, &pe32));

  CloseHandle(process_snapshot);

  return pid;
}

int main(int argc, char *argv[]) {
  int sig, pid;
  int pos, is_pid;
  // might as well only print description when invoked solo
  if (argc == 1) {
    printf(desc_string);
  }
  if (argc <= 2) {
    printf(usage_string);
    return 1;
  }
  // get our signal/command type
  if (strcmp(sig_strings[0], argv[1]) == 0) {
    sig = CTRL_C_EVENT;
  } else if (strcmp(sig_strings[1], argv[1]) == 0) {
    sig = CTRL_BREAK_EVENT;
  } else if (strcmp(sig_strings[2], argv[1]) == 0) {
    sig = 2;
  } else {
    printf(usage_string);
    return 1;
  }
  // find out if this is a PID
  pos = 0;
  is_pid = true;
  while (argv[2][pos++] != '\0') {
    if (!isdigit(argv[2][pos])) {
      is_pid = false;
      break;
    }
  }
  // acquire a PID either way
  if (is_pid) {
    pid = atoi(argv[2]);
  } else {
    pid = findPID(argv[2]);
    if (pid == 0) {
      printf("ERROR: Failed to find PID for %s\n", argv[2]);
      return 2;
    }
  }
  // let's send our signal!
  printf("%s: %d\n", argv[1], pid);
  if (sig < 2) { // SIGINT, SIGBREAK
    FreeConsole();
    if (AttachConsole(pid) == 0) {
      printf("ERROR: AttachConsole failed!\n");
      return 2;
    }
    SetConsoleCtrlHandler(NULL, TRUE);  // disable signal handling
    GenerateConsoleCtrlEvent(sig, 0);
  } else { // kill
    HANDLE explorer;
    explorer = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    TerminateProcess(explorer, 1);
    CloseHandle(explorer);
  }
  return 0;
}

