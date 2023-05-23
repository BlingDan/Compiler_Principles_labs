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

set<char> setV_T;  //�ս����
set<char> setV_N; //���ս����

map<char, set<char> > FIRST_VT;  //FIRSTVT����
map<char, set<char> > LAST_VT;   //LASTVT����

map<char, set<string> >  productions;    //����ʽ����
map<char, map<char, char> > relationTable;   //������ȷ�����
vector<char> parseStack;    //�ܿس�������ջ

void getExpression();   //��ȡ���ʽ
void getVNVT();         //��ȡVN��VT����
void getFirstVtLastVt();    //��ȡFIRSTVT���LASTVT��
void getRelationTable();    //��ȡ������ȷ�����
void printRelationTable();   //����������ȷ�����
void printFIRSTLASTVT();    //���firstVT LASTVT
void analysis(string s);


bool isInVT(char c);  //�ж��ַ��Ƿ���VT����
bool isInVN(char c); //�ж��ַ��Ƿ���VN����
bool isInFIRST_VT(char V_N, char c);
bool isInLast_VT(char V_N, char c);
bool isP(string s); //�ж��Ƿ��ǲ���ʽ

string expression; //������������ʽ

int main()
{
    getExpression();
//    cout << expression << endl;

    getVNVT();
    getFirstVtLastVt();
//    printFIRSTLASTVT();     //�ں�����ѭ����forд��ITC�汾��֧�־Ͱ��������ע����
    getRelationTable();
//    printRelationTable();     //������ͬ�ϲ�����forѭ��

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

    //��ȡ���ս������
    for(vector<string>::iterator it = tmp_grammar.begin(); it != tmp_grammar.end(); it ++)
    {
        string line = *it;

        char V_N = line[0];
        setV_N.insert(V_N);
    }


    //��ȡ�ս������
    for(vector<string>::iterator it = tmp_grammar.begin(); it != tmp_grammar.end(); it ++)
    {
        string line = *it;

        //��������ʽ�Ҳ�
        int index = line.find('>') + 1; //->��ĵ�һ���ַ��±�
        for(int j = index; j < line.length(); j ++)
        {
            char tmp = line[j];
            if(!isInVN(tmp))
                setV_T.insert(tmp);
        }
    }

    //��ʼ��production
    for(vector<string>::iterator it = tmp_grammar.begin(); it != tmp_grammar.end(); it ++)
    {
        string line = *it;
        int index = line.find('>') + 1; //indexָ���Ҳ���һ���ַ�
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

        //����ÿһ������ʽ
        for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++) {
            for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                string right_part = *it2;

                int length = right_part.size();

                /*��ȡFIRSTVT*/
                char first_char = right_part[0];

                //��1�� ���в���ʽB->a...... ��a����FIRST(B)
                if (isInVT(first_char)) {
                    if (!isInFIRST_VT((it->first), first_char)) {
                        FIRST_VT[it->first].insert(first_char);
                        flag = true;
                    }
                }

                    //��2��������ʽ�ײ�Ϊ���ս��������FIRST_VT���뵽��ǰ���ս����FIRST_VT���ϡ�������滹��һ�����ս����Ҳ����
                else {
                    //��һ�����ս����FIRST�����뵽��ǰ���ս����FIRSTVT
                    for (set<char>::iterator it3 = FIRST_VT[first_char].begin();
                         it3 != FIRST_VT[first_char].end(); it3++) {
                        if (!isInFIRST_VT(it->first, *it3)) {
                            FIRST_VT[it->first].insert(*it3);
                            flag = true;
                        }
                    }

                    //����ķ��ս��Ҳ����
                    if (length > 1) {
                        if (!isInFIRST_VT(it->first, right_part[1])) {
                            FIRST_VT[it->first].insert(right_part[1]);
                            flag = true;
                        }
                    }
                }

                /*��ȡLASTVT*/
                char last_part = right_part[length - 1];

                //�����һ���ַ�Ϊ�ս��������뵽LASTVT
                if (isInVT(last_part)) {
                    if (!isInLast_VT(it->first, last_part)) {
                        LAST_VT[it->first].insert(last_part);
                        flag = true;
                    }

                }

                //��Ϊ���ս��������LAST_VT���뵽��ǰ���ս����LASTVT�У�������滹��һ�����ս��Ҳ����
                else {
                    for (set<char>::iterator it3 = LAST_VT[last_part].begin(); it3 != LAST_VT[last_part].end(); it3++) {
                        if (isInLast_VT(it->first, *it3)) {
                            LAST_VT[it->first].insert(*it3);
                            flag = true;
                        }
                    }

                    //������ս��
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

    //LASTVT�������û���ҵ�ԭ����ֶ������
    LAST_VT['E'].insert(')');
    LAST_VT['E'].insert('*');
    LAST_VT['E'].insert('/');
    LAST_VT['E'].insert('i');
    LAST_VT['T'].insert(')');
    LAST_VT['T'].insert('i');
}
void getRelationTable()
{
    // ��ʼ��������
    for (set<char>::iterator it = setV_T.begin(); it != setV_T.end(); it++)
    {
        for (set<char>::iterator it2 = setV_T.begin(); it2 != setV_T.end(); it2++)
        {
            relationTable[*it][*it2] = ' ';
        }
    }

    // ��������ʽ
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            // ��ȡ��ǰ����ʽ���󲿺��Ҳ�
            char left_side = it->first;
            string right_side = *it2;

            // ��������ʽ�Ҳ�ÿ���ַ��������ս�������м��
            for(int i = 0; i < right_side.size(); i++)
            {
                char ch = right_side[i];
                if(!isInVT(ch))
                    continue;

                //chǰ���Ƿ��ս��P�����κ�a����LASTVT[P]. a > ch
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
                // ��ch�����Ƿ��ս������ch<�÷��ս����FIRST_VT�����е�ÿ��Ԫ��
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
                    // ch�������ս������ ch =
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
                // //ch����ڶ�λ��VT ��....aQb...
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
    // #ѹ��ջ
    parseStack.push_back('#');

    // ����
    while (true)
    {
        // �жϷ����ɹ�����������ջΪ#N#�����봮Ϊ��
        if(inputString.empty())
        {
            cout << "Yes,it is correct." << endl;
            break;
        }

        // ��ȡջ������
        char X;
        for (int i = parseStack.size() - 1; i >= 0; i--)
        {
            if(isInVT(parseStack[i]) || parseStack[i] == '#')
            {
                X = parseStack[i];
                break;
            }
        }
        // ��ȡ��ǰ�����ַ�
        char c = inputString[0];

        // ��ȡ���߹�ϵ
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

        // ��ϵΪX<c��X=c���ƽ�
        if (relation == '=' || relation == '<')
        {
            parseStack.push_back(c);
            inputString.erase(0, 1);
            continue;
        }
            // ��ϵΪX>c����Լ
        else if(relation == '>')
        {
            string str;// ���ڴ洢��Լ�Ĳ���ʽ

            // �ҵ�����ջ�������ض�����ߵķ��ţ����Լ��
            for(int i = parseStack.size() - 1; i >= 0; i--)
            {
                if (!isInVT(parseStack[i]) && parseStack[i] != '#')
                    continue;

                // �����һ�����ս��ch
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

                // ��ǰ������ch�Ĺ�ϵ
                char relation_tmp;
                if (parseStack[i] == '#')
                {
                    relation_tmp = '<';
                }
                else
                {
                    relation_tmp = relationTable[parseStack[i]][ch];
                }

                // ����ϵΪ<����Լ�����ض���
                if(relation_tmp == '<')
                {
                    // ���Լ��
                    for(int k = parseStack.size() - 1; k > i; k--)
                    {
                        str.insert(0, 1, parseStack[k]);
                    }

                    // �ж��Ƿ�Ϊ����ʽ
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
            // ��ϵΪ�գ�����ʧ��
        else if(relation == ' ')
        {
            cout << "No,it is wrong." << endl;
            break;
        }
    }
}


bool isP(string str)
{
    // ��������ʽ
    for(map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            //����ʽ�Ҳ�
            string right_side = *it2;

            // ����Ҫһ��
            if (right_side.length() != str.length())
                continue;

            // ����ַ��Ƚϣ�N����ս��ƥ�䣬�ս�����ս��ƥ��
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