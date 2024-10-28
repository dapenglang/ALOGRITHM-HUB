#include <iostream>
using namespace std;
#include <string>

#define MAXN 1010
#define MAX_MEMORY 32768
#define TIME_LIMIT 1000000
#define LEN_LIMIT 40

struct oper{
    string str; //存放操作数的字符串形式
    int kind;   //操作数种类，用1..4标识
    /*
        1 02C0  立即数
        2 AX    寄存器
        3 T02C0 内存+立即数
        4 TAX   内存+寄存器
    */
    unsigned short value;
    /*
        立即数：存储立即数的值
        寄存器：用0..3标识AX..DX
        内存+立即数：存储立即数的值
        内存+寄存器：用0..3标识TAX..TDX
    */
    bool check()    //检查该操作数格式是否正确，并根据str对kind和value进行初始化
    {
        int len = str.length();
        if(len==2)
        {
            value = 4;
            if(str == "AX") value = 0;
            if(str == "BX") value = 1;
            if(str == "CX") value = 2;
            if(str == "DX") value = 3;
            if(value == 4) return false;
            kind = 2;
            return true;
        }
        if(len==3)
        {
            value = 4;
            if(str == "TAX") value = 0;
            if(str == "TBX") value = 1;
            if(str == "TCX") value = 2;
            if(str == "TDX") value = 3;
            if(value == 4) return false;
            kind = 4;
            return true;
        }
        if(len < 4 || len > 5) return false;
        int base = 0;
        if(str[0] == 'T') base = 1;
        if(len == base + 4)
        {
            value = 0;
            for(int i=0; i<4; i++)
            {
                char ch = str[base+i];
                unsigned short digit;
                if(ch >= '0' && ch <= '9')
                {
                    digit = ch - '0';
                }
                else if(ch >= 'A' && ch <= 'F')
                {
                    digit = ch - 'A' + 10;
                }
                else return false;
                value = value * 16 + digit;
            }
            if(base == 0) kind = 1;
            else kind = 3;
            return true;
        }
        else return false;
    }
};

struct instruction{
    string name;    //存放指令首部的字符串形式
    int kind;       //1..7分别对应RUN到CMP，80..86对应JMP到JNE
    oper op1,op2;   //两个操作数
    bool check()    //检查该指令格式是否正确，并根据name初始化kind，读入所需操作数
    {
        if(name == "RUN")
        {
            kind = 1;
            return true;
        }
        if(name == "STOP")
        {
            kind = 2;
            return true;
        }
        if(name == "ECHO")
        {
            kind = 3;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "ADD")
        {
            kind = 4;
            cin >> op1.str >> op2.str;
            return op1.check() && (op1.kind != 1) && op2.check() && (op1.kind < 3 || op2.kind < 3);
        }
        if(name == "INC")
        {
            kind = 5;
            cin >> op1.str;
            return op1.check() && (op1.kind != 1);
        }
        if(name == "MOV")
        {
            kind = 6;
            cin >> op1.str >> op2.str;
            return op1.check() && (op1.kind != 1) && op2.check() && (op1.kind < 3 || op2.kind < 3);
        }
        if(name == "CMP")
        {
            kind = 7;
            cin >> op1.str >> op2.str;
            return op1.check() && op2.check() && (op1.kind < 3 || op2.kind < 3);
        }
        if(name == "JMP")
        {
            kind = 80;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "JG")
        {
            kind = 81;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "JL")
        {
            kind = 82;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "JE")
        {
            kind = 83;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "JNG")
        {
            kind = 84;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "JNL")
        {
            kind = 85;
            cin >> op1.str;
            return op1.check();
        }
        if(name == "JNE")
        {
            kind = 86;
            cin >> op1.str;
            return op1.check();
        }
        return false;
    }
};

instruction code[MAXN]; // 存放所有指令
unsigned short memory[MAX_MEMORY];  // [3000,B000) 内存数据段，仅使用低8位，用unsigned short类型是为了处理方便
unsigned short reg[4];  // 寄存器 0 AX  1 BX  2 CX  3 DX
bool cmp = false;       // 执行过CMP指令后设为True
unsigned short aa,bb;   // 存放CMP指令两个操作数中的值
int nnow = 0;
unsigned short std_ans,stu_ans;
int echo_times = 0;
int score = 0;
int cnt = 0;

bool read_data(oper op,unsigned short &data)    // 读取操作数op中的值，存入data
{
    if(op.kind == 1)
    {
        data = op.value;
        return true;
    }
    if(op.kind == 2)
    {
        data = reg[op.value];
        return true;
    }
    if(op.kind == 3)
    {
        if(op.value < 12288 || op.value >= 45056 - 1)
        {
            cout << "ACCESS_VIOLATION" << endl;
            return false;
        }
        unsigned short tmp = op.value - 12288;
        data = memory[tmp] + memory[tmp+1] * 256;
        return true;
    }
    if(op.kind == 4)
    {
        unsigned short tmp = reg[op.value];
        if(tmp < 12288 || tmp >= 45056 - 1)
        {
            cout << "ACCESS_VIOLATION" << endl;
            cout << "at line " << nnow << endl;
            return false;
        }
        tmp -= 12288;
        data = memory[tmp] + memory[tmp+1] * 256;
        return true;
    }
    cerr << "PE - Wrong Operator" << endl;
    return false;
}

bool write_data(oper op,unsigned short data)    // 将操作数op中的值修改为data
{
    if(op.kind == 2)
    {
        reg[op.value] = data;
        return true;
    }
    if(op.kind == 3)
    {
        if(op.value < 12288 || op.value >= 45056 - 1)
        {
            cout << "ACCESS_VIOLATION" << endl;
            return false;
        }
        unsigned short tmp = op.value - 12288;
        memory[tmp] = data & 255;
        memory[tmp+1] = data >> 8;
        return true;
    }
    if(op.kind == 4)
    {
        unsigned short tmp = reg[op.value];
        if(tmp < 12288 || tmp >= 45056 - 1)
        {
            cout << "ACCESS_VIOLATION" << endl;
            cout << "at line " << nnow << endl;
            return false;
        }
        tmp -= 12288;
        memory[tmp] = data & 255;
        memory[tmp+1] = data >> 8;
        return true;
    }
    cerr << "PE - Wrong Operator" << endl;
    return false;
}

void output(unsigned short data)    // 按四位十六进制格式输出data
{
    unsigned short tmp;
    char ch[4];
    for(int i=0; i<4; i++)
    {
        tmp = data & 15;
        data = data >> 4;
        if(tmp<10) ch[i] = '0' + tmp;
        else ch[i] = 'A' + tmp - 10;
    }
    for(int i=3; i>=0; i--)
        cout << ch[i];
    cout << endl;
}

bool work()
{
    //先检查格式问题
    int n = 0;  //指令条数，从code[1]开始存放指令
    while(cin >> code[n+1].name)
    {
        n++;
        if(code[n].check() == false)
        {
            cerr << "PE at " << n << " line" << endl;
            return false;
        }
        //cerr << code[n].op1.kind << "\t" << code[n].op1.value << "\t" << code[n].op2.kind << "\t" << code[n].op2.value << endl;
        if(n == LEN_LIMIT) break;   // 读入LEN_LIMIT条指令后自动退出
    }
    if(code[n].kind != 2 || code[1].kind != 1)  // 检查第一条和最后一条指令是否为RUN和STOP
    {
        cerr << "PE at 1 or " << n << " line" << endl;
        return false;;
    }
    for(int i=2; i<n; i++)  // 检查是否有多余的RUN和STOP
        if(code[i].kind < 3)
    {
        cerr << "PE at " << i << " line" << endl;
        return false;
    }
    // 此时保证格式正确
    unsigned short next = 2, i = 1;
    unsigned short a,b,x;
    bool ok = false;    // ok为TRUE表示程序通过STOP正常结束
    cmp = false;
    while(true)
    {
        nnow = i;
        int kind = code[i].kind;
        if(kind == 2)   // STOP
        {
            ok = true;
            break;
        }
        if(kind == 3)   // ECHO A
        {
            if(read_data(code[i].op1, a))
            {
                stu_ans = a;
                echo_times++;
            }
            else break;
        }
        if(kind == 4)   // ADD A B
        {
            if(!read_data(code[i].op1, a)) break;
            if(!read_data(code[i].op2, b)) break;
            a += b;
            if(!write_data(code[i].op1, a)) break;
        }
        if(kind == 5)  // INC A
        {
            if(!read_data(code[i].op1, a)) break;
            a++;
            if(!write_data(code[i].op1, a)) break;
        }
        if(kind == 6)   // MOV A B
        {
            if(!read_data(code[i].op2, b)) break;
            if(!write_data(code[i].op1, b)) break;
        }
        if(kind == 7)   // CMP A B
        {
            if(!read_data(code[i].op1, aa)) break;
            if(!read_data(code[i].op2, bb)) break;
            cmp = true;
        }
        if(kind >= 80)  // JMP X
        {
            if(kind > 80 && !cmp)
            {
                cout << "CMP_MISSING" << endl;
                break;
            }
            if(!read_data(code[i].op1, x)) break;
            bool flag = false;
            if(kind == 80) flag = true;
            if(kind == 81 && aa > bb) flag = true;
            if(kind == 82 && aa < bb) flag = true;
            if(kind == 83 && aa == bb) flag = true;
            if(kind == 84 && aa <= bb) flag = true;
            if(kind == 85 && aa >= bb) flag = true;
            if(kind == 86 && aa != bb) flag = true;
            if(flag)
            {
                if(x<2 || x>n)
                {
                    cout << "RUNTIME_ERROR" << endl;
                    break;
                }
                next = x;
            }
        }
        cnt++;
        if(cnt == TIME_LIMIT)
        {
            if(code[i].kind != 2) cout << "TLE" << endl;
            break;
        }
        i = next;
        next = i + 1;
    }
    return ok;
}

void init()
{
    unsigned short i;
    unsigned short tmp;
    bool used[5050] = {0};
    std_ans = 0;
    tmp = 1;
    for(i=0; i<10000; i+=2)
    {
        if(i<5000) tmp = tmp * 13 + 7;
        else tmp = tmp * 7 + 13;
        tmp %= 5000;
        memory[i] = tmp & 255;
        memory[i+1] = tmp >> 8;
        if(!used[tmp])
        {
            used[tmp] = true;
            std_ans++;
        }
    }
    //cout << std_ans << endl;
    reg[0] = 5000;
    score = 10;
}

int main()
{
    init();
    if(!work()) score = 0;
    if(echo_times != 1) score = 0;
    if(std_ans != stu_ans) score = 0;
    cout << "ECHO " << echo_times << " time(s)" << endl;
    cout << "Time_CNT = " << cnt << endl;
    if(echo_times == 1)
    {
        cout << "Student's answer = " << stu_ans << endl;
        cout << "Standard  answer = " << std_ans << endl;
    }
    cout << "Score = " << score << endl;
    return 0;
}
