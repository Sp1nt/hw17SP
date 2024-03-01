#include "mainDlg.h"

mainDlg* mainDlg::ptr = NULL;
CRITICAL_SECTION cs;
HANDLE hMutex;
HWND hEdit1;

mainDlg::mainDlg(void) {
	ptr = this;
}

mainDlg::~mainDlg(void) {
	DeleteCriticalSection(&cs);
}

void mainDlg::Cls_OnClose(HWND hwnd) {
	EndDialog(hwnd, 0);
}

BOOL mainDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	InitializeCriticalSection(&cs);

	return TRUE;
}

DWORD WINAPI WriteToFilesThread(LPVOID lp) {

	EnterCriticalSection(&cs);

	
	std::wifstream in(_TEXT("data.txt", std::ios::in));

	int N = 6;


	if (!in) {
		MessageBox(0, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}

	wchar_t buff[254];

	for (int i = 1; i <= N; i++) {

		wsprintf(buff, _TEXT("copy_data_%d.txt"), i);

		std::wofstream out(buff, std::ios::out);

		while (!in.eof()) {
			in.getline(buff, 254);

			out << buff << "\r\n";
		}

		in.clear();

		in.seekg(0, std::ios::beg);

		out.close();
	}

	in.close();


	LeaveCriticalSection(&cs);

	MessageBox(0, _T("Копии созданы!"), _T("Info"), MB_OK);

	return 0;
}

DWORD WINAPI ReadFromFilesThread(LPVOID lp) {

	EnterCriticalSection(&cs);

	std::wofstream out("full_data.txt", std::ios::out);

	int N = 4;

	wchar_t buff[254];

	for (int i = 1; i <= N; i++) {
		wsprintf(buff, _TEXT("copy_data_%d.txt"), i);

		std::wifstream in(buff, std::ios::in);

		if (!in) {
			MessageBox(0, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
			return TRUE;
		}

		while (!in.eof()) {
			in.getline(buff, 254);

			out << buff << "\n";
		}

		in.clear();

		in.seekg(0, std::ios::beg);
	}

	out.close();

	LeaveCriticalSection(&cs);

	MessageBox(0, _T("Запись завершена!"), _T("Info"), MB_OK);

	return 0;
}

void mainDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	if (id == IDC_BUTTON1) {

		HANDLE h;
		h = CreateThread(NULL, 0, WriteToFilesThread, 0, 0, NULL);
		CloseHandle(h);
		h = CreateThread(NULL, 0, ReadFromFilesThread, 0, 0, NULL);
		CloseHandle(h);
	}
}

BOOL CALLBACK mainDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}

	return FALSE;
}