#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;


string expression;  //��Ҫ�жϵľ���
map<char, int> V2index;
set<char> setV_T;  //�ս����
set<char> setV_N; //���ս����


void getExpression();                           //���������ն˵�����
void initGrammer(vector<string> &tmp_grammar); //��ʼ���ķ�
bool analysis(string str, int table[][9], vector<string> grammers);            //�ܿس���
void translationTable();
string num2str(int num);
int str2num(string str);
bool isInVT(char c);        //�ж��Ƿ����ս������
bool isInVN(char c);        //�ж��Ƿ��ڷ��ս������


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

    vector<string> grammers;    //�ķ��ĸ�����ʽ
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

}
string translationTable(int row, int col, int table[][9]) {

    int num = table[row][col];

    if (col >= 0 && col <= 5)    //Action��
    {
        if (num == 0)
            return "null";
        if (num == 100)
            return "acc";
        if (num > 100)
            return "r" + num2str(num - 100);
        if (num < 100 && num > 0)
            return "s" + num2str(num);
    } else {                       //GOTO��
        if (num == 0)
            return "null";
        else
            return num2str(num);
    }
}

bool analysis(string str, int table[][9], vector<string> grammers)
{

    str += '#';
    //��ʼ������ջ��״̬ջ
    vector<char> sign;      //����ջ
    vector<int> status;    //״̬ջ
    sign.push_back('#');
    status.push_back(0);

    int index = 0;
    int str_length = str.length();
    while(true)
    {
        char currentChar = str[index];  //��ǰ���봮���ַ�
        string op = translationTable(status.back(), V2index[currentChar], table);
//        cout << "row:"<< status.back()  << "\tcol:" << V2index[currentChar] << "\tvalue:"<<op << endl;
        if(op[0] == 'a')
            return true;
        else if(op[0] == 'n') {
//            cout << "ACTION error" << endl;
            return false;
        }
        else if(op[0] == 's'){   //�ƽ�����
//            cout << "s:" << currentChar << endl;
            sign.push_back(currentChar);
            status.push_back(str2num(op.substr(1)));
            index ++;
        }else if(op[0] == 'r'){  //��Լ����
            int grammer_num = str2num(op.substr(1));
            string currnetGrammer = grammers[grammer_num - 1];
//            cout << "r:" << currnetGrammer << endl;
            char rightPart = currnetGrammer[0];
            string leftPart = currnetGrammer.substr(3);

            for(int i = 0; i < leftPart.length(); i ++){    //��ȥջ��length����
                sign.pop_back();
                status.pop_back();
            }

            sign.push_back(rightPart);//�ַ�����ջ
//            cout << V2index[rightPart] << endl;
            string tmp = translationTable(status.back(), V2index[rightPart], table);
            if(tmp == "null"){
//                cout << "GOTO error" << endl;
                return false;
            }
            else{
                status.push_back(str2num(tmp));  //��GOTO���¼��״̬��ջ
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