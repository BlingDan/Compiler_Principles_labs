#include <iostream>
#include <string>
#include <stdio.h>
//#include <fstream>
//#include <cstring>

#define GET_ARRAY_LEN(array,len) {len = (sizeof(array) / sizeof(array[0]));}

using namespace std;

const string base_keywords[] = {"begin", "call", "const", "do", "end",
                                "if", "odd", "procedure", "read",
                                "then", "var", "while", "write"};

//判断是否是基本字
bool Letter(string str)
{
    int len;
    GET_ARRAY_LEN(base_keywords, len);
    for(int i = 0; i < len; i ++)
    {
        if(str.compare(base_keywords[i]) == 0)
            return true;
    }
    return false;
}
int main() {

    string str;
    char ch;

    while((ch = getchar()) != EOF) {

//        cout << "ch--->" << ch << endl;

        //若为空格或者换行符，跳过
        if (ch == ' ' || ch == '\n')
            continue;

            //识别基本字或标识符
        else if(isalpha(ch))
        {
            str = "";
            while(isalpha(ch) || isdigit(ch))
            {
                str += ch;
                ch = getchar(); //
            }
            ungetc(ch,stdin);   //将字符回退到输入流
            if(Letter(str))
                cout << "(" << str << "sym" << "," << str << ")" << endl;
            else
                cout << "(ident," << str << ")" << endl;
        }

            //识别常数 没有判断浮点数环节
        else if(isdigit(ch))
        {
            str = "";
            while(isdigit(ch))
            {
                str += ch;
                ch = getchar();
            }
            ungetc(ch,stdin);
            cout << "(number,"<< str << ")" << endl;
        }

            //识别运算符
        else if(ch == '+') cout << "(plus,+)" << endl;
        else if(ch == '-') cout << "(minus,-)" << endl;
        else if(ch == '*') cout << "(times,*)" << endl;
        else if(ch == '/') cout << "(slash,/)" << endl;
        else if(ch == '=') cout << "(eql,=)" << endl;
        else if(ch == '<')
        {
            ch = getchar();
            if(ch == '>')   cout << "(neq,<>)" << endl;
            else if(ch == '=') cout << "(leq,<=)" << endl;
            else {
                cout << "(lss,<)" << endl;
                ungetc(ch,stdin);
            }
        }
        else if(ch == '>')
        {
            ch = getchar();
            if(ch == '=') cout << "(geq,>=)" << endl;
            else {
                cout << "(gtr,>)" << endl;
                ungetc(ch,stdin);
            }
        }

        else if(ch == ':')   //只有赋值号的情况
        {
            ch = getchar();
            cout << "(becomes,:=)" << endl;
        }

            //识别界符
        else if(ch == '(') cout << "(lparen,()" << endl;
        else if(ch == ')') cout << "(rparen,))" << endl;
        else if(ch == ',') cout << "(comma,,)" << endl;
        else if(ch == ';') cout << "(semicolon,;)" << endl;
        else if(ch == '.') cout << "(period,.)" << endl;


    }

    return 0;
}



