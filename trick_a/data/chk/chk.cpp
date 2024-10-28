#include <iostream>
using namespace std;
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>

#define MAXN 1010
#define MAX_MEMORY 32768
#define TIME_LIMIT 1000000
#define LEN_LIMIT 40

unsigned short memory[MAX_MEMORY];  // [3000,B000) 内存数据段，仅使用低8位，用unsigned short类型是为了处理方便
unsigned short reg[4];  // 寄存器 0 AX  1 BX  2 CX  3 DX
bool cmp = false;       // 执行过CMP指令后设为True
unsigned short aa,bb;   // 存放CMP指令两个操作数中的值
unsigned short std_ans,stu_ans;
int echo_times = 0;
int cnt = 0;
int code_len = 0;
ifstream fin,stu_in,std_in;       // 用于读取标准读入文件
int task_type = 0;  // 任务类型 1 2 3
int data_num = 0;   // 数据组数
int correct = 0;    // 正确通过的组数
ofstream fout;      // 用于输出具体错误信息

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
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "ADD")
        {
            kind = 4;
            stu_in >> op1.str >> op2.str;
            return op1.check() && (op1.kind != 1) && op2.check() && (op1.kind < 3 || op2.kind < 3);
        }
        if(name == "INC")
        {
            kind = 5;
            stu_in >> op1.str;
            return op1.check() && (op1.kind != 1);
        }
        if(name == "MOV")
        {
            kind = 6;
            stu_in >> op1.str >> op2.str;
            return op1.check() && (op1.kind != 1) && op2.check() && (op1.kind < 3 || op2.kind < 3);
        }
        if(name == "CMP")
        {
            kind = 7;
            stu_in >> op1.str >> op2.str;
            return op1.check() && op2.check() && (op1.kind < 3 || op2.kind < 3);
        }
        if(name == "JMP")
        {
            kind = 80;
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "JG")
        {
            kind = 81;
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "JL")
        {
            kind = 82;
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "JE")
        {
            kind = 83;
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "JNG")
        {
            kind = 84;
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "JNL")
        {
            kind = 85;
            stu_in >> op1.str;
            return op1.check();
        }
        if(name == "JNE")
        {
            kind = 86;
            stu_in >> op1.str;
            return op1.check();
        }
        return false;
    }
};

instruction code[MAXN]; // 存放所有指令

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
            fout << "ACCESS_VIOLATION" << endl;
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
            fout << "ACCESS_VIOLATION" << endl;
            return false;
        }
        tmp -= 12288;
        data = memory[tmp] + memory[tmp+1] * 256;
        return true;
    }
    fout << "PE - Wrong Operator" << endl;
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
            fout << "ACCESS_VIOLATION" << endl;
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
            fout << "ACCESS_VIOLATION" << endl;
            return false;
        }
        tmp -= 12288;
        memory[tmp] = data & 255;
        memory[tmp+1] = data >> 8;
        return true;
    }
    fout << "PE - Wrong Operator" << endl;
    return false;
}

bool work()
{
    // 此时保证格式正确
    unsigned short next = 2, i = 1;
    unsigned short a,b,x;
    bool ok = false;    // ok为TRUE表示程序通过STOP正常结束
    cmp = false;
    while(true)
    {
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
                fout << "CMP_MISSING" << endl;
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
                if(x < 2 || x > code_len)
                {
                    fout << "RUNTIME_ERROR" << endl;
                    break;
                }
                next = x;
            }
        }
		/*if(cnt % 100000 == 0){
			printf("cnt = %d\n", cnt);
			fflush(stdout);
		}*/
        cnt++;
        if(cnt == TIME_LIMIT)
        {
            if(code[i].kind != 2) fout << "TLE" << endl;
            break;
        }
        i = next;
        next = i + 1;
    }
    return ok;
}

void clr()  // 内存初始化
{
    memset(memory,0,sizeof(memory));
    memset(reg,0,sizeof(reg));
    cmp = false;
    echo_times = 0;
    cnt = 0;
}

void init()     // 将已知数据导入内存
{
    int i,j;
    int n,tmp;
    if(task_type < 0 || task_type > 3)
    {
        cerr << "task_type error" << endl;
        exit(0);
    }
    if(task_type == 3)
    {
        fin >> reg[0] >> reg[1];
        if(reg[0] <= 0 || reg[0] > 200 || reg[1] < 0 || reg[1] > reg[0])
        {
            cerr << "data error: n = " << reg[0] << "\tm = " << reg[1] << endl;
            exit(0);
        }
    }
    else{
        fin >> n;
        if(n <= 0 || n > 16384)
        {
            cerr << "data error: n = " << n << endl;
            exit(0);
        }
        j = 0;
        for(i=0; i<n; i++)
        {
            fin >> tmp;
            memory[j] = tmp & 255;
            memory[j+1] = tmp >> 8;
            j += 2;
        }
        if(task_type == 1) reg[0] = j - 2 + 12288;
        else reg[0] = n;
    }
    std_in >> std_ans;
}

bool read_stu_code()
{
    //先检查格式问题
    code_len = 0;  //指令条数，从code[1]开始存放指令
    while(stu_in >> code[code_len+1].name)
    {
        code_len++;
        if(code[code_len].check() == false)
        {
            fout << "PE at " << code_len << " line" << endl;
            return false;
        }
        if(code_len == LEN_LIMIT) break;   // 读入LEN_LIMIT条指令后自动退出
    }
    if(code_len == 0)
    {
        fout << "PE" << endl;
        return false;
    }
    if(code[code_len].kind != 2 || code[1].kind != 1)  // 检查第一条和最后一条指令是否为RUN和STOP
    {
        fout << "PE at 1 or " << code_len << " line" << endl;
        return false;
    }
    for(int i=2; i<code_len; i++)  // 检查是否有多余的RUN和STOP
        if(code[i].kind < 3)
    {
        fout << "PE at " << i << " line" << endl;
        return false;
    }
    return true;
}

int main(int argc,char** argv)
{
    fin.open(argv[1]);
    stu_in.open(argv[2]);
    std_in.open(argv[3]);
    fout.open(argv[6]);
    fin >> task_type >> data_num;
    fout << "Task " << task_type << endl;
    if(read_stu_code())
    {
        for(int i=0; i<data_num; i++)
        {
			printf("data = %d\n", i);fflush(stdout);
            clr();
            init();
            fout << "test " << i+1 << ":\t";
            if(!work()) continue;
            if(echo_times != 1 || std_ans != stu_ans)
            {
                fout << "WA" << endl;
                continue;
            }
            else{
            	correct++;
            	fout << "AC" << endl;
            }
        }
    }
    fin.close();
    fout.close();
    stu_in.close();
    std_in.close();
    fout.open(argv[5]);
    fout << 100 * correct / data_num << endl;
    fout.close();
    return 0;
}
