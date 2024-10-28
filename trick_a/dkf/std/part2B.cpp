#include <iostream>
using namespace std;
#include <string>

#define MAXN 1010
#define MAX_MEMORY 32768
#define TIME_LIMIT 1000000
#define LEN_LIMIT 40

struct oper{
    string str; //\B4\E6\B7Ų\D9\D7\F7\CA\FD\B5\C4\D7ַ\FB\B4\AE\D0\CEʽ
    int kind;   //\B2\D9\D7\F7\CA\FD\D6\D6\C0࣬\D3\C31..4\B1\EAʶ
    /*
        1 02C0  \C1\A2\BC\B4\CA\FD
        2 AX    \BCĴ\E6\C6\F7
        3 T02C0 \C4ڴ\E6+\C1\A2\BC\B4\CA\FD
        4 TAX   \C4ڴ\E6+\BCĴ\E6\C6\F7
    */
    unsigned short value;
    /*
        \C1\A2\BC\B4\CA\FD\A3\BA\B4洢\C1\A2\BC\B4\CA\FD\B5\C4ֵ
        \BCĴ\E6\C6\F7\A3\BA\D3\C30..3\B1\EAʶAX..DX
        \C4ڴ\E6+\C1\A2\BC\B4\CA\FD\A3\BA\B4洢\C1\A2\BC\B4\CA\FD\B5\C4ֵ
        \C4ڴ\E6+\BCĴ\E6\C6\F7\A3\BA\D3\C30..3\B1\EAʶTAX..TDX
    */
    bool check()    //\BC\EC\B2\E9\B8ò\D9\D7\F7\CA\FD\B8\F1ʽ\CAǷ\F1\D5\FDȷ\A3\AC\B2\A2\B8\F9\BE\DDstr\B6\D4kind\BA\CDvalue\BD\F8\D0г\F5ʼ\BB\AF
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
    string name;    //\B4\E6\B7\C5ָ\C1\EE\CAײ\BF\B5\C4\D7ַ\FB\B4\AE\D0\CEʽ
    int kind;       //1..7\B7ֱ\F0\B6\D4ӦRUN\B5\BDCMP\A3\AC80..86\B6\D4ӦJMP\B5\BDJNE
    oper op1,op2;   //\C1\BD\B8\F6\B2\D9\D7\F7\CA\FD
    bool check()    //\BC\EC\B2\E9\B8\C3ָ\C1\EE\B8\F1ʽ\CAǷ\F1\D5\FDȷ\A3\AC\B2\A2\B8\F9\BE\DDname\B3\F5ʼ\BB\AFkind\A3\AC\B6\C1\C8\EB\CB\F9\D0\E8\B2\D9\D7\F7\CA\FD
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

instruction code[MAXN]; // \B4\E6\B7\C5\CB\F9\D3\D0ָ\C1\EE
unsigned short memory[MAX_MEMORY];  // [3000,B000) \C4ڴ\E6\CA\FD\BEݶΣ\AC\BD\F6ʹ\D3õ\CD8λ\A3\AC\D3\C3unsigned short\C0\E0\D0\CD\CA\C7Ϊ\C1˴\A6\C0\ED\B7\BD\B1\E3
unsigned short reg[4];  // \BCĴ\E6\C6\F7 0 AX  1 BX  2 CX  3 DX
bool cmp = false;       // ִ\D0й\FDCMPָ\C1\EE\BA\F3\C9\E8ΪTrue
unsigned short aa,bb;   // \B4\E6\B7\C5CMPָ\C1\EE\C1\BD\B8\F6\B2\D9\D7\F7\CA\FD\D6е\C4ֵ
int nnow = 0;
unsigned short std_ans,stu_ans;
int echo_times = 0;
int score = 0;
int cnt = 0;

bool read_data(oper op,unsigned short &data)    // \B6\C1ȡ\B2\D9\D7\F7\CA\FDop\D6е\C4ֵ\A3\AC\B4\E6\C8\EBdata
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

bool write_data(oper op,unsigned short data)    // \BD\AB\B2\D9\D7\F7\CA\FDop\D6е\C4ֵ\D0޸\C4Ϊdata
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

void output(unsigned short data)    // \B0\B4\CB\C4λʮ\C1\F9\BD\F8\D6Ƹ\F1ʽ\CA\E4\B3\F6data
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
    //\CFȼ\EC\B2\E9\B8\F1ʽ\CE\CA\CC\E2
    int n = 0;  //ָ\C1\EE\CC\F5\CA\FD\A3\AC\B4\D3code[1]\BF\AAʼ\B4\E6\B7\C5ָ\C1\EE
    while(cin >> code[n+1].name)
    {
        n++;
        if(code[n].check() == false)
        {
            cerr << "PE at " << n << " line" << endl;
            return false;
        }
        //cerr << code[n].op1.kind << "\t" << code[n].op1.value << "\t" << code[n].op2.kind << "\t" << code[n].op2.value << endl;
        if(n == LEN_LIMIT) break;   // \B6\C1\C8\EBLEN_LIMIT\CC\F5ָ\C1\EE\BA\F3\D7Զ\AF\CD˳\F6
    }
    if(code[n].kind != 2 || code[1].kind != 1)  // \BC\EC\B2\E9\B5\DAһ\CC\F5\BA\CD\D7\EE\BA\F3һ\CC\F5ָ\C1\EE\CAǷ\F1ΪRUN\BA\CDSTOP
    {
        cerr << "PE at 1 or " << n << " line" << endl;
        return false;;
    }
    for(int i=2; i<n; i++)  // \BC\EC\B2\E9\CAǷ\F1\D3ж\E0\D3\E0\B5\C4RUN\BA\CDSTOP
        if(code[i].kind < 3)
    {
        cerr << "PE at " << i << " line" << endl;
        return false;
    }
    // \B4\CBʱ\B1\A3֤\B8\F1ʽ\D5\FDȷ
    unsigned short next = 2, i = 1;
    unsigned short a,b,x;
    bool ok = false;    // okΪTRUE\B1\EDʾ\B3\CC\D0\F2ͨ\B9\FDSTOP\D5\FD\B3\A3\BD\E1\CA\F8
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
				cerr << a << endl;
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
		if (i == 0) continue;
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
