#include <stdio.h>
#include <stdlib.h>

// ハリボテ言語HL-1
// 機能: 変数宣言、数値の代入、加算、減算、変数の値の表示

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

void handleError(unsigned char *s)
{
    printf("syntax error: %.10s\n", s);
    exit(1);
}

int main(int argc, const char **argv)
{
    const int TXT_MAX_SIZE = 100000;
    unsigned char txt[TXT_MAX_SIZE];
    int var[256];
    loadText(argc, argv, txt, TXT_MAX_SIZE);
    // NOTE: 数値も変数として管理することで、定数と変数の区別が不要になる
    for (int i = 0; i < 10; i++)
    {
        var['0' + i] = i;
    }
    for (int pc = 0; txt[pc] != 0; pc++)
    {
        if (
            txt[pc] == '\n' ||
            txt[pc] == '\r' ||
            txt[pc] == ' ' ||
            txt[pc] == '\t' ||
            txt[pc] == ';')
        {
            continue;
        }
        else if ( // 単純代入
            txt[pc + 1] == '=' &&
            txt[pc + 3] == ';')
        {
            var[txt[pc]] = var[txt[pc + 2]];
        }
        else if ( // 加算
            txt[pc + 1] == '=' &&
            txt[pc + 3] == '+' &&
            txt[pc + 5] == ';')
        {
            var[txt[pc]] = var[txt[pc + 2]] + var[txt[pc + 4]];
        }
        else if ( //減算
            txt[pc + 1] == '=' &&
            txt[pc + 3] == '-' &&
            txt[pc + 5] == ';')
        {
            var[txt[pc]] = var[txt[pc + 2]] - var[txt[pc + 4]];
        }
        else if ( // print文
            txt[pc] == 'p' &&
            txt[pc + 1] == 'r' &&
            txt[pc + 5] == ' ' &&
            txt[pc + 7] == ';')
        {
            printf("%d\n", var[txt[pc + 6]]);
        }
        else
        {
            handleError(&txt[pc]);
        }
        while (txt[pc] != ';')
        {
            pc++;
        }
    }
    exit(0);
}