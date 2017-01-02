// -----------------------------------------------------------------------------------------
// aup_reseter by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2013-2017 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <stdio.h>

typedef struct {
    const char *header;
    DWORD pos;
} status_flag_t;

//対応プロジェクトファイルリスト
//ここに追加していく
const status_flag_t AUP_LIST[] = {
    { "AviUtl ProjectFile version 0.18", 0x26 }
};

int reset_aup_file(const char *aupfile) {
    BYTE flag = 0;
    FILE *fp = NULL;
    if (0 != fopen_s(&fp, aupfile, "rb+") || fp == NULL) {
        fprintf(stderr, "オープンに失敗: %s\n", PathFindFileName(aupfile));
        return 1;
    }
    int result = 0;
    char buf[128] = { 0 };
    fread(buf, sizeof(buf), 1, fp);
    int idx;
    for (idx = _countof(AUP_LIST) - 1; idx >= 0; idx--)
        if (0 == strcmp(buf, AUP_LIST[idx].header))
            break;
    if (idx < 0) {
        fprintf(stderr, "書き換え可能なファイルではありません: %s\n", PathFindFileName(aupfile));
        result = 1;
    } else if (0 != fseek(fp, AUP_LIST[idx].pos, SEEK_SET)) {
        fprintf(stderr, "書き換えに失敗: %s\n", PathFindFileName(aupfile));
        result = 1;
    } else if (1 != fwrite(&flag, sizeof(flag), 1, fp)) {
        fprintf(stderr, "書き換えに失敗: %s\n", PathFindFileName(aupfile));
        result = 1;
    }
    fclose(fp);
    if (!result)
        fprintf(stderr, "成功: %s\n", PathFindFileName(aupfile));
    return result;
}

void print_help() {
    fprintf(stderr, 
        "aup_reseter.exe by rigaya\n"
        "Aviutlのaupプロジェクトファイルの終了フラグをリセットします。\n"
        "\n"
        " Aviutlフォルダに置いてダブルクリックすれば、\n"
        " フォルダ内の全てのbatchxx.aupの終了フラグをリセットします。\n"
        "\n"
        " あるいは、リセットしたいbatchxx.aupをドラッグドロップしてください。\n");
}

int main(int argc, char **argv) {
    int ret = 0;
    int count = 0;
    if (argc == 1) {
        WIN32_FIND_DATA win32fd = { 0 };
        HANDLE hnd = FindFirstFile(".\\batch*.aup", &win32fd);
        if (hnd != INVALID_HANDLE_VALUE) {
            do {
                if (0 == (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    ret |= reset_aup_file(win32fd.cFileName);
                    count++;
                }
            } while (FindNextFile(hnd, &win32fd));
        }
    } else {
        for (int i_arg = 1; i_arg < argc; i_arg++) {
            ret |= reset_aup_file(argv[i_arg]);
            count++;
        }
    }
    if (count == 0)
        print_help();
    return ret;
}
