#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef unsigned char *String;

#define MAX_TC 1000
String ts[MAX_TC + 1];                  // トークンの文字列を記録 トークンコードからトークン文字列を得るために使う
int tl[MAX_TC + 1];                     // トークンの長さ トークンコードからトークン文字列の長さを得るために使う
unsigned char tcBuf[(MAX_TC + 1) * 10]; // トークン文字列の実体を格納(1トークンあたり平均10バイトを想定)
int tcs = 0;                            // 記録済みのトークンの個数
int tcb = 0;                            // tcBufの未使用領域の先頭オフセット
int var[MAX_TC + 1];                    // 変数 key: トークンコード, value: 変数の値
int tc[100000];                         // トークンコード

void loadText(int argc, const char **argv, unsigned char *t, int size)
{
    FILE *fp;
    int i;
    if (argc < 2)
    {
        printf("usage>%s program-file\n", argv[0]);
        exit(1);
    }
    fp = fopen(argv[1], "rt");
    if (fp == 0)
    {
        printf("fopen error: %s\n", argv[1]);
        exit(1);
    }
    i = fread(t, 1, size - 1, fp);
    fclose(fp);
    t[i] = 0; // NULL終端
}

int getTc(String s, int len)
{
    int i;
    // sが登録済みのトークンの場合はトークンコードを返却する
    for (i = 0; i < tcs; i++)
    {
        if (len == tl[i] && strncmp(s, ts[i], len) == 0)
        {
            break;
        }
    }
    if (i == tcs)
    {
        // sが未登録のトークンの場合、新規登録を行う
        if (tcs >= MAX_TC)
        {
            printf("too many tokens\n");
            exit(1);
        }
        strncpy(&tcBuf[tcb], s, len); // tcBufに新規登録
        tcBuf[tcb + len] = 0;
        ts[i] = &tcBuf[tcb];
        tl[i] = len;
        tcb += len + 1;
        tcs++;
        var[i] = strtol(ts[i], 0, 0); // 定数の場合には初期値を設定
    }
    return i;
}

int isAlphabetOrNumber(unsigned char c)
{
    if ('0' <= c && c <= '9')
        return 1;
    if ('a' <= c && c <= 'z')
        return 1;
    if ('A' <= c && c <= 'Z')
        return 1;
    if (c == '_')
        return 1;
    return 0;
}

int lexer(String s, int tc[])
{
    int i = 0; // 現在,文字列sのどこを読んでいるか
    int j = 0; // これまでに変換したトークン列の長さ
    int len;
    while (true)
    {
        if (s[i] == ' ' || s[i] == '\r' || s[i] == '\t' || s[i] == '\n')
        {
            i++;
            continue;
        }
        if (s[i] == 0)
        {
            return j;
        }
        len = 0; // トークン文字列の長さ
        if (strchr("(){}[];,", s[i]) != 0)
        {
            len = 1;
        }
        else if (isAlphabetOrNumber(s[i]))
        {
            while (isAlphabetOrNumber(s[i + len]))
            {
                len++;
            }
        }
        else if (strchr("=+-*/!%&~|<>?:.#", s[i]) != 0)
        {
            while (strchr("=+-*/!%&~|<>?:.#", s[i + len]) != 0 && s[i + len] != 0)
            {
                len++;
            }
        }
        else
        {
            printf("syntax error: %.10s\n", &s[i]);
            exit(1);
        }
        tc[j] = getTc(&s[i], len);
        i += len;
        j++;
    }
}

int main(int argc, const char **argv)
{
    int pc, pc1;
    unsigned char txt[10000]; // ソースコード用のバッファ.
    loadText(argc, argv, txt, 10000);
    pc1 = lexer(txt, tc);
    tc[pc1] = tc[pc1 + 1] = tc[pc1 + 2] = tc[pc1 + 3] = getTc(".", 1); // エラー表示用のために末尾にピリオドを登録しておく.
    int semi = getTc(";", 1);
    for (pc = 0; pc < pc1; pc++)
    { // プログラム実行開始.
        if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == semi)
        { // 単純代入.
            var[tc[pc]] = var[tc[pc + 2]];
        }
        else if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == getTc("+", 1) && tc[pc + 5] == semi)
        { // 加算.
            var[tc[pc]] = var[tc[pc + 2]] + var[tc[pc + 4]];
        }
        else if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == getTc("-", 1) && tc[pc + 5] == semi)
        { // 減算.
            var[tc[pc]] = var[tc[pc + 2]] - var[tc[pc + 4]];
        }
        else if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == getTc("*", 1) && tc[pc + 5] == semi)
        { // 乗算.
            var[tc[pc]] = var[tc[pc + 2]] * var[tc[pc + 4]];
        }
        else if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == getTc("/", 1) && tc[pc + 5] == semi)
        { // 除算.
            var[tc[pc]] = var[tc[pc + 2]] / var[tc[pc + 4]];
        }
        else if (tc[pc] == getTc("print", 5) && tc[pc + 2] == semi)
        { // print.
            printf("%d\n", var[tc[pc + 1]]);
        }
        else
            goto err;
        while (tc[pc] != semi)
            pc++;
    }
    exit(0);
err:
    printf("syntax error : %s %s %s %s\n", ts[tc[pc]], ts[tc[pc + 1]], ts[tc[pc + 2]], ts[tc[pc + 3]]);
    exit(1);
}