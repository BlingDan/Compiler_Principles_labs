#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;


string expression;  //需要判断的句子
map<char, int> V2index;
set<char> setV_T;  //终结符集
set<char> setV_N; //非终结符集


void getExpression();                           //接收来自终端的输入
void initGrammer(vector<string> &tmp_grammar); //初始化文法
bool analysis(string str, int table[][9], vector<string> grammers);            //总控程序
void translationTable();
string num2str(int num);
int str2num(string str);
bool isInVT(char c);        //判断是否在终结符集中
bool isInVN(char c);        //判断是否在非终结符集中


int main()
{
    int LR_table[12][9] = {
            {0,0,4,0,5,0,1,2,3},
            {6,0,0,0,0,100,0,0,0},
            {102,7,0,102,0,102,0,0,0},
            {104,104,0,104,0,104,0,0,0},
            {0,0,4,0,5,0,8,2,3},
            {106,106,0,106,0,106,0,0,0},
            {0,0,4,0,5,0,0,9,3},
            {0,0,4,0,5,0,0,0,10},
            {6,0,0,11,0,0,0,0,0},
            {102,107,0,101,0,101,0,0,0},
            {103,103,0,103,0,103,0,0,0},
            {105,105,0,105,0,105,0,0,0},


    };

    vector<string> grammers;    //文法的各产生式
    initGrammer(grammers);

    getExpression();
//    string str = "(i+i)+*i";
    bool flag = analysis(expression, LR_table, grammers);


    return 0;

}

void getExpression()
{

    string line;

    while(cin >> line)
    {
        int index = line.find(',');

        string left_part = line.substr(1, index - 1);
        string right_part = line.substr(index + 1, line.length() - index - 2);

        if(left_part == "ident" || left_part == "number")
            right_part = "i";

        expression += right_part;
    }
    return;
}
void initGrammer(vector<string> &tmp_grammar)
{

    char Vt[11] = {'+','*','(',')','i','#','E','T','F'};
    for(int i = 0; i < 9; i ++){
        V2index[Vt[i]] = i;
    }

    tmp_grammar.push_back("E->E+T");
    tmp_grammar.push_back("E->T");
    tmp_grammar.push_back("T->T*F");
    tmp_grammar.push_back("T->F");
    tmp_grammar.push_back("F->(E)");
    tmp_grammar.push_back("F->i");

    //获取非终结符集合
    for(vector<string>::iterator it = tmp_grammar.begin(); it != tmp_grammar.end(); it ++)
    {
        string line = *it;

        char V_N = line[0];
        setV_N.insert(V_N);
    }


    //获取终结符集合
    for(vector<string>::iterator it = tmp_grammar.begin(); it != tmp_grammar.end(); it ++)
    {
        string line = *it;

        //遍历产生式右部
        int index = line.find('>') + 1; //->后的第一个字符下标
        for(int j = index; j < line.length(); j ++)
        {
            char tmp = line[j];
            if(!isInVN(tmp))
                setV_T.insert(tmp);
        }
    }

}
string translationTable(int row, int col, int table[][9]) {

    int num = table[row][col];

    if (col >= 0 && col <= 5)    //Action表
    {
        if (num == 0)
            return "null";
        if (num == 100)
            return "acc";
        if (num > 100)
            return "r" + num2str(num - 100);
        if (num < 100 && num > 0)
            return "s" + num2str(num);
    } else {                       //GOTO表
        if (num == 0)
            return "null";
        else
            return num2str(num);
    }
}

bool analysis(string str, int table[][9], vector<string> grammers)
{

    str += '#';
    //初始化符号栈，状态栈
    vector<char> sign;      //符号栈
    vector<int> status;    //状态栈
    sign.push_back('#');
    status.push_back(0);

    int index = 0;
    int str_length = str.length();
    while(true)
    {
        char currentChar = str[index];  //当前输入串首字符
        string op = translationTable(status.back(), V2index[currentChar], table);
//        cout << "row:"<< status.back()  << "\tcol:" << V2index[currentChar] << "\tvalue:"<<op << endl;
        if(op[0] == 'a')
            return true;
        else if(op[0] == 'n') {
//            cout << "ACTION error" << endl;
            return false;
        }
        else if(op[0] == 's'){   //移进操作
//            cout << "s:" << currentChar << endl;
            sign.push_back(currentChar);
            status.push_back(str2num(op.substr(1)));
            index ++;
        }else if(op[0] == 'r'){  //规约操作
            int grammer_num = str2num(op.substr(1));
            string currnetGrammer = grammers[grammer_num - 1];
//            cout << "r:" << currnetGrammer << endl;
            char rightPart = currnetGrammer[0];
            string leftPart = currnetGrammer.substr(3);

            for(int i = 0; i < leftPart.length(); i ++){    //除去栈顶length个项
                sign.pop_back();
                status.pop_back();
            }

            sign.push_back(rightPart);//字符左部入栈
//            cout << V2index[rightPart] << endl;
            string tmp = translationTable(status.back(), V2index[rightPart], table);
            if(tmp == "null"){
//                cout << "GOTO error" << endl;
                return false;
            }
            else{
                status.push_back(str2num(tmp));  //将GOTO表记录的状态入栈
            }
        }
    }
}

bool isInVT(char c)
{
    return setV_T.count(c);
}
bool isInVN(char c)
{
    return setV_N.count(c);
}

string num2str(int num)
{
    string tmp =  "";
    tmp += char(num + '0');
    return tmp;
}

int str2num(string str)
{
    int ans = 0;
    for(int i = 0; i < str.length() ; i ++)
        ans += ans * 10 + (str[i] - '0');

    return ans;
}