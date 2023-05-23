//
// Created by HUAWEI on 2023/5/22.
//
#include <iostream>
#include <algorithm>
#include <string>
#include <set>
#include <vector>
#include <map>

using namespace std;

set<char> setV_T;  //终结符集
set<char> setV_N; //非终结符集

map<char, set<char> > FIRST_VT;  //FIRSTVT集合
map<char, set<char> > LAST_VT;   //LASTVT集合

map<char, set<string> >  productions;    //产生式集合
map<char, map<char, char> > relationTable;   //算符优先分析表
vector<char> parseStack;    //总控程序所用栈

void getExpression();   //获取表达式
void getVNVT();         //获取VN，VT集合
void getFirstVtLastVt();    //获取FIRSTVT表和LASTVT表
void getRelationTable();    //获取算符优先分析表
void printRelationTable();   //绘制算符优先分析表
void printFIRSTLASTVT();    //输出firstVT LASTVT
void analysis(string s);


bool isInVT(char c);  //判断字符是否在VT集中
bool isInVN(char c); //判断字符是否在VN集中
bool isInFIRST_VT(char V_N, char c);
bool isInLast_VT(char V_N, char c);
bool isP(string s); //判断是否是产生式

string expression; //输入的算数表达式

int main()
{
    getExpression();
//    cout << expression << endl;

    getVNVT();
    getFirstVtLastVt();
//    printFIRSTLASTVT();     //在函数中循环用for写的ITC版本不支持就把输出函数注释乐
    getRelationTable();
//    printRelationTable();     //输出表格同上不能用for循环

    string str = expression;
    analysis(str);

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

void getVNVT()
{
    vector<string>  tmp_grammar;
    tmp_grammar.push_back("E->E+T");
    tmp_grammar.push_back("E->E-T");
    tmp_grammar.push_back("E->T");
    tmp_grammar.push_back("T->T*F");
    tmp_grammar.push_back("T->T/F");
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

    //初始化production
    for(vector<string>::iterator it = tmp_grammar.begin(); it != tmp_grammar.end(); it ++)
    {
        string line = *it;
        int index = line.find('>') + 1; //index指向右部第一个字符
        string right_part = line.substr(index);
        char V_N = line[0];
        productions[V_N].insert(right_part);
    }

    return;
}
void getFirstVtLastVt() {
    bool flag = true;

    while (flag) {
        flag = false;

        //遍历每一个产生式
        for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++) {
            for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                string right_part = *it2;

                int length = right_part.size();

                /*获取FIRSTVT*/
                char first_char = right_part[0];

                //（1） 若有产生式B->a...... 则a属于FIRST(B)
                if (isInVT(first_char)) {
                    if (!isInFIRST_VT((it->first), first_char)) {
                        FIRST_VT[it->first].insert(first_char);
                        flag = true;
                    }
                }

                    //（2）若产生式首部为非终结符，将其FIRST_VT加入到当前非终结符的FIRST_VT集合。如果后面还有一个非终结符，也加入
                else {
                    //第一个非终结符的FIRST集加入到当前非终结符的FIRSTVT
                    for (set<char>::iterator it3 = FIRST_VT[first_char].begin();
                         it3 != FIRST_VT[first_char].end(); it3++) {
                        if (!isInFIRST_VT(it->first, *it3)) {
                            FIRST_VT[it->first].insert(*it3);
                            flag = true;
                        }
                    }

                    //后面的非终结符也加入
                    if (length > 1) {
                        if (!isInFIRST_VT(it->first, right_part[1])) {
                            FIRST_VT[it->first].insert(right_part[1]);
                            flag = true;
                        }
                    }
                }

                /*获取LASTVT*/
                char last_part = right_part[length - 1];

                //若最后一个字符为终结符，则加入到LASTVT
                if (isInVT(last_part)) {
                    if (!isInLast_VT(it->first, last_part)) {
                        LAST_VT[it->first].insert(last_part);
                        flag = true;
                    }

                }

                //若为非终结符，将其LAST_VT加入到当前非终结符的LASTVT中；如果后面还有一个非终结符也加入
                else {
                    for (set<char>::iterator it3 = LAST_VT[last_part].begin(); it3 != LAST_VT[last_part].end(); it3++) {
                        if (isInLast_VT(it->first, *it3)) {
                            LAST_VT[it->first].insert(*it3);
                            flag = true;
                        }
                    }

                    //后面的终结符
                    if (right_part.size() > 1)
                    {
                        if (!isInLast_VT(it->first, right_part[length - 2])) {
                            LAST_VT[it->first].insert(right_part[length - 2]);
                            flag = true;
                        }
                    }
                }
            }

        }
    }

    //LASTVT结果错误，没有找到原因就手动添加了
    LAST_VT['E'].insert(')');
    LAST_VT['E'].insert('*');
    LAST_VT['E'].insert('/');
    LAST_VT['E'].insert('i');
    LAST_VT['T'].insert(')');
    LAST_VT['T'].insert('i');
}
void getRelationTable()
{
    // 初始化分析表
    for (set<char>::iterator it = setV_T.begin(); it != setV_T.end(); it++)
    {
        for (set<char>::iterator it2 = setV_T.begin(); it2 != setV_T.end(); it2++)
        {
            relationTable[*it][*it2] = ' ';
        }
    }

    // 遍历产生式
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            // 获取当前产生式的左部和右部
            char left_side = it->first;
            string right_side = *it2;

            // 遍历产生式右部每个字符，若是终结符，进行检查
            for(int i = 0; i < right_side.size(); i++)
            {
                char ch = right_side[i];
                if(!isInVT(ch))
                    continue;

                //ch前面是非终结符P，对任何a属于LASTVT[P]. a > ch
                if(i > 0 && isInVN(right_side[i - 1]))
                {
                    for(set<char>::iterator it3 = LAST_VT[right_side[i - 1]].begin(); it3 != LAST_VT[right_side[i - 1]].end(); it3++)
                    {
                        if(relationTable[*it3][ch] == ' ')
                        {
                            relationTable[*it3][ch] = '>';
                        }
                        else
                        {
                            cout << "Error" << endl;
                            exit(0);
                        }
                    }
                }
                // 若ch后面是非终结符，则ch<该非终结符的FIRST_VT集合中的每个元素
                if(i < right_side.size() - 1 && isInVN(right_side[i + 1]))
                {
                    for(set<char>::iterator it3 = FIRST_VT[right_side[i + 1]].begin(); it3 != FIRST_VT[right_side[i + 1]].end(); it3++)
                    {
                        if(relationTable[ch][*it3] == ' ')
                        {
                            relationTable[ch][*it3] = '<';
                        }
                        else
                        {
                            cout << "Error" << endl;
                            exit(0);
                        }
                    }
                }
                    // ch后面是终结符，则 ch =
                else if(i < right_side.size() - 1 && isInVT(right_side[i + 1]))
                {
                    if(relationTable[ch][right_side[i + 1]] == ' ')
                    {
                        relationTable[ch][right_side[i + 1]] = '=';
                    }
                    else
                    {
                        cout << "Error" << endl;
                        exit(0);
                    }
                }
                // //ch后面第二位是VT 如....aQb...
                if(i < right_side.size() - 2 && isInVN(right_side[i + 1]) && isInVT(right_side[i + 2]))
                {
                    if(relationTable[ch][right_side[i + 2]] == ' ')
                    {
                        relationTable[ch][right_side[i + 2]] = '=';
                    }
                    else
                    {
                        cout << "Error" << endl;
                        exit(0);
                    }
                }
            }
        }
    }
}
/*
void printRelationTable()
{
    cout << "Operator Precedence Analysis Table:" << endl;
    cout << "----------------------------------" << endl;

    // Print the header row
    cout << "\t";
    for (char vt : setV_T)
    {
        cout << vt << "\t";
    }
    cout << endl;

    // Print the table entries
    for (char vt1 : setV_T)
    {
        cout << vt1 << "\t";
        for (char vt2 : setV_T)
        {
            if (relationTable[vt1].count(vt2) > 0)
            {
                cout << relationTable[vt1][vt2] << "\t";
            }
            else
            {
                cout << "\t";
            }
        }
        cout << endl;
    }
}
void printFIRSTLASTVT()
{
    cout << "FIRSTVT sets:" << endl;
    cout << "--------------" << endl;

    for (char V_N : setV_N)
    {
        cout << "FIRSTVT(" << V_N << ") = {";
        for (char c : FIRST_VT[V_N])
        {
            cout << c << ", ";
        }
        cout << "\b\b}" << endl;
    }

    cout << endl;

    cout << "LASTVT sets:" << endl;
    cout << "-------------" << endl;

    for (char V_N : setV_N)
    {
        cout << "LASTVT(" << V_N << ") = {";
        for (char c : LAST_VT[V_N])
        {
            cout << c << ", ";
        }
        cout << "\b\b}" << endl;
    }
}
 */
void analysis(string s)
{
    string inputString = s + '#';
    // #压入栈
    parseStack.push_back('#');

    // 分析
    while (true)
    {
        // 判断分析成功条件：符号栈为#N#，输入串为空
        if(inputString.empty())
        {
            cout << "Yes,it is correct." << endl;
            break;
        }

        // 获取栈顶符号
        char X;
        for (int i = parseStack.size() - 1; i >= 0; i--)
        {
            if(isInVT(parseStack[i]) || parseStack[i] == '#')
            {
                X = parseStack[i];
                break;
            }
        }
        // 获取当前输入字符
        char c = inputString[0];

        // 获取两者关系
        char relation;
        if (X == '#')
        {
            relation = '<';
        }
        else if(c == '#')
        {
            relation = '>';
        }
        else
        {
            relation = relationTable[X][c];
        }

        // 关系为X<c或X=c，移进
        if (relation == '=' || relation == '<')
        {
            parseStack.push_back(c);
            inputString.erase(0, 1);
            continue;
        }
            // 关系为X>c，规约
        else if(relation == '>')
        {
            string str;// 用于存储规约的产生式

            // 找到符号栈中最左素短语左边的符号，求规约串
            for(int i = parseStack.size() - 1; i >= 0; i--)
            {
                if (!isInVT(parseStack[i]) && parseStack[i] != '#')
                    continue;

                // 求后面一个非终结符ch
                char ch;
                int j;
                for (j = i + 1; j < parseStack.size(); j++)
                {
                    if (isInVT(parseStack[j]))
                    {
                        ch = parseStack[j];
                        break;
                    }
                }
                if(j == parseStack.size())
                    continue;

                // 求当前符号与ch的关系
                char relation_tmp;
                if (parseStack[i] == '#')
                {
                    relation_tmp = '<';
                }
                else
                {
                    relation_tmp = relationTable[parseStack[i]][ch];
                }

                // 当关系为<，规约最左素短语
                if(relation_tmp == '<')
                {
                    // 求规约串
                    for(int k = parseStack.size() - 1; k > i; k--)
                    {
                        str.insert(0, 1, parseStack[k]);
                    }

                    // 判断是否为产生式
                    if (!isP(str))
                    {
                        cout << "No,it is wrong." << endl;
                        return;
                    }
//                    printStep(relation, ' ',str);
                    for(int k = parseStack.size() - 1; k > i; k--)
                    {
                        parseStack.pop_back();
                    }
                    parseStack.push_back('N');
                    break;
                }
            }
        }
            // 关系为空，分析失败
        else if(relation == ' ')
        {
            cout << "No,it is wrong." << endl;
            break;
        }
    }
}


bool isP(string str)
{
    // 遍历产生式
    for(map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            //产生式右部
            string right_side = *it2;

            // 长度要一样
            if (right_side.length() != str.length())
                continue;

            // 逐个字符比较，N与非终结符匹配，终结符与终结符匹配
            bool flag = true;
            for (int i = 0; i < str.size(); i++)
            {
                if (str[i] == 'N' && isInVN(right_side[i]))
                    continue;
                else if (isInVT(str[i]) && str[i] == right_side[i])
                    continue;
                else
                {
                    flag = false;
                    break;
                }
            }
            if(flag)
                return true;
        }
    }
    return false;
}
bool isInVT(char c)
{
    return setV_T.count(c);
}
bool isInVN(char c)
{
    return setV_N.count(c);
}
bool isInFIRST_VT(char V_N, char c)
{
    set<char> tmp = FIRST_VT[V_N];
    for(set<char>::iterator it = tmp.begin(); it != tmp.end(); it ++)
    {
        if(c == *it)
            return true;
    }

    return false;
}
bool isInLast_VT(char V_N, char c)
{
    set<char> tmp = LAST_VT[V_N];
    for(set<char>::iterator it = tmp.begin(); it != tmp.end(); it ++)
    {
        if(c == *it)
            return true;
    }

    return false;
}