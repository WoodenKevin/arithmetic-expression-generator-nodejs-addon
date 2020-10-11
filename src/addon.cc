#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include <map>
#include <stack>
#include <string>

#include <napi.h>

using namespace std;

map<int32_t, char> op; // 运算符 ID 及其符号映射
map<int32_t, int> pri; // 运算符 ID 及其优先级映射

// 运算数结构体
// id 为运算数的位置，val 为运算数的值
struct node
{
    int32_t id, val;
    node(int32_t id = -1, int32_t val = -1) : id(id), val(val) {}
};

stack<int32_t> opr; // 中缀表达式转后缀表达式时的运算符栈
stack<node> opd;    // 中缀表达式转后缀表达式时的运算数栈

int32_t tot;         // 逆波兰表达式的长度
int32_t operatorNum; // 运算符个数
int32_t operandNum;  // 运算数的个数
int32_t bracketNum;  // 括号对数
int32_t ans;         // 运算式最终答案

int32_t hasBracket[15];   // 某个数字处是否有括号，-1表示无括号，14表示左括号，15表示右括号
int32_t operators[15];    // 对应位置的运算符种类
int32_t operands[15];     // 表示对应位置运算数的值
int32_t RPN[55];          // 存储逆波兰表达式
int32_t fac[105];         // 存储某个数的约数
int32_t numerators[15];   // 对应位置运算数的分子
int32_t denominators[15]; // 对应位置运算式的分母

napi_value res; // Store result object

void InitRes(napi_env env)
{
    napi_status status;

    // create a JavaScript Array res
    status = napi_create_array(env, &res);
    assert(status == napi_ok);
}

void AddRes(napi_env env, const char *expression, const char *answer)
{
    napi_status status;

    // check whether res is a JavaScript Array
    bool is_array;
    status = napi_is_array(env, res, &is_array);
    assert(status == napi_ok);

    if (is_array)
    {
        // get length of the JavaScript Array res
        uint32_t array_lenth;
        status = napi_get_array_length(env, res, &array_lenth);
        assert(status == napi_ok);

        // create a JavaScript Object obj
        napi_value obj;
        status = napi_create_object(env, &obj);
        assert(status == napi_ok);

        // create a JavaScript String with expression
        napi_value obj_expression;
        status = napi_create_string_utf8(env, expression, NAPI_AUTO_LENGTH, &obj_expression);
        assert(status == napi_ok);

        // create a new property named 'expression'
        // with the JavaScript String obj_expression
        // for the JavaScript Object obj
        status = napi_set_named_property(env, obj, "expression", obj_expression);
        assert(status == napi_ok);

        // create a JavaScript String with answer
        napi_value obj_answer;
        status = napi_create_string_utf8(env, answer, NAPI_AUTO_LENGTH, &obj_answer);
        assert(status == napi_ok);

        // create a new property named 'answer'
        // with the JavaScript String obj_answer
        // for the JavaScript Object obj
        status = napi_set_named_property(env, obj, "answer", obj_answer);
        assert(status == napi_ok);

        // push JavaScript Object obj
        // into JavaScript Array res
        status = napi_set_element(env, res, array_lenth, obj);
        assert(status == napi_ok);
    }
}

// 初始化运算符及其优先级的映射
// 10 表示加号   运算优先级为 0
// 11 表示减号   运算优先级为 0
// 12 表示乘号   运算优先级为 1
// 13 表示除号   运算优先级为 1
// 14 表示左括号
// 15 表示右括号
void InitMap()
{
    op[10] = '+';
    op[11] = '-';
    op[12] = '*';
    op[14] = '(';
    op[15] = ')';

    pri[10] = 0;
    pri[11] = 0;
    pri[12] = 1;
    pri[13] = 1;
}

// 在每次调用 Solve() 时进行初始化
void Solve1Init()
{
    tot = 0;
    for (int32_t i = 0; i < 15; i++)
    {
        operators[i] = -1;
        operands[i] = -1;
        hasBracket[i] = -1;
    }
    while (!opr.empty())
        opr.pop();
    while (!opd.empty())
        opd.pop();
}

// 在每次调用 getOperands() 时进行初始化
void GetOperandsInit()
{
    tot = 0;
    while (!opr.empty())
        opr.pop();
    while (!opd.empty())
        opd.pop();
}

// 在每次调用 solve2() 时进行初始化
void Solve2Init()
{
    tot = 0;
    for (int32_t i = 0; i < 15; i++)
    {
        operators[i] = -1;
        numerators[i] = -1;
        denominators[i] = -1;
    }
}

// 求两数最大公约数
int32_t GCD(int32_t a, int32_t b)
{
    while (b)
    {
        int32_t t = b;
        b = a % b;
        a = t;
    }

    return a;
}

// 生成一个范围在 [l, r] 中的随机数
int32_t GetNum(int32_t l, int32_t r)
{
    int32_t ret = rand() % (r - l + 1) + l;

    return ret;
}

// 获取运算数的值的函数，如果返回 true 则表示获取成功，否则表示获取失败
// 生成范围为 0~k 的运算数
bool GetOperands(int32_t k)
{
    GetOperandsInit();

    // 将中缀表达式转换成后缀表达式（也就是逆波兰表达式）
    if (hasBracket[0] != -1)
        opr.push(hasBracket[0]);

    RPN[tot++] = 0;

    for (int32_t i = 1; i < operandNum; ++i)
    {
        while (true)
        {
            if (opr.empty() || opr.top() == 14 || pri[operators[i - 1]] > pri[opr.top()])
            {
                opr.push(operators[i - 1]);
                break;
            }
            RPN[tot++] = opr.top();
            opr.pop();
        }
        if (hasBracket[i] == 14)
        {
            opr.push(hasBracket[i]);
        }
        RPN[tot++] = i;
        if (hasBracket[i] == 15)
        {
            while (opr.top() != 14)
            {
                RPN[tot++] = opr.top();
                opr.pop();
            }
            opr.pop();
        }
    }

    while (!opr.empty())
    {
        RPN[tot++] = opr.top();
        opr.pop();
    }

    // 转换成逆波兰表达式后便可以进行尝试填数
    for (int32_t i = 0; i < tot; ++i)
    {
        // 如果为运算数则随机为其赋值
        if (RPN[i] < 10)
        {
            int32_t x = GetNum(1, k);
            operands[RPN[i]] = x;
            opd.push(node(RPN[i], x));

            continue;
        }
        //如果为除法，要将除数随机分配为被除数的一个因子
        //如果为减法，要注意减数不能大于被减数

        if (RPN[i] == 13)
        {
            node b = opd.top();
            opd.pop();
            node a = opd.top();
            opd.pop();

            if (a.val % b.val == 0)
            {
                opd.push(node(-1, a.val / b.val));

                continue;
            }

            if (b.id == -1)
                return false;

            int32_t cnt = 0;
            for (int32_t j = 1; j <= a.val; ++j)
            {
                if (j >= 100)
                    break;
                if (a.val % j)
                    continue;
                fac[cnt++] = j;
            }

            int32_t x = GetNum(0, cnt - 1);
            operands[b.id] = fac[x];
            opd.push(node(-1, a.val / fac[x]));
        }
        else if (RPN[i] == 11)
        {
            node b = opd.top();
            opd.pop();
            node a = opd.top();
            opd.pop();

            int32_t dt = a.val - b.val;
            if (dt <= 0)
                return false;
            opd.push(node(-1, dt));
        }
        else
        {
            node b = opd.top();
            opd.pop();
            node a = opd.top();
            opd.pop();

            if (RPN[i] == 10)
                opd.push(node(-1, a.val + b.val));
            if (RPN[i] == 12)
                opd.push(node(-1, a.val * b.val));
        }
    }

    ans = opd.top().val;
    opd.pop();

    //控制最终运算结果的范围，可根据需要进行调节
    if (ans < 0 || ans > 1000)
        return false;
    return true;
}

// 生成普通运算式
bool Solve1(napi_env env, bool flag, int32_t k)
{
    Solve1Init();

    string expression = "", answer = "";

    // 随机生成运算符的个数 1~3 ，及运算数个数 2~4
    operatorNum = GetNum(1, 3);
    operandNum = operatorNum + 1;

    // 随机生成括号个数
    if (operandNum == 2)
        bracketNum = 0;
    else
        bracketNum = min((int32_t)(operandNum / 2), GetNum(1, 2));
    if (flag)
        bracketNum = 0;
    // 随机生成运算符的种类
    for (int32_t i = 0; i < operatorNum; i++)
        operators[i] = GetNum(10, 13);
    // 随机生成括号位置
    for (int32_t i = 0; i < bracketNum * 2; i++)
    {
        int32_t x = GetNum(0, operandNum - 1);
        while (hasBracket[x] != -1)
            x = GetNum(0, operandNum - 1);
        hasBracket[x] = 0;
    }

    // 根据相对位置确定括号为左括号还是右括号
    bool lf = true;
    for (int32_t i = 0; i < operandNum; ++i)
    {
        if (hasBracket[i] == -1)
            continue;
        if (lf)
            hasBracket[i] = 14;
        else
            hasBracket[i] = 15;
        lf = (!lf);
    }

    // 到这里已经将等式预处理成了(a+b)/c*(d-e)的类似形式
    // 预处理结束之后，我们就要尝试将 a b c d e 确定为具体的数
    if (!GetOperands(k))
        return false;

    // 在获取完运算数之后，便可以输出我们得到的等式了
    if (hasBracket[0] != -1)
        expression += "(";

    expression += to_string(operands[0]);

    for (int32_t i = 1; i < operandNum; i++)
    {
        if (operators[i - 1] == 13)
            expression += "÷";
        else
            expression += op[operators[i - 1]];

        if (hasBracket[i] == 14)
            expression += "(";

        expression += to_string(operands[i]);

        if (hasBracket[i] == 15)
            expression += ")";
    }

    answer = to_string(ans);

    AddRes(env, expression.c_str(), answer.c_str());

    return true;
}

// 生成真分数运算式
// 由于只要求加减法，因此括号出现与否并不会影响答案
// 故本函数中无须考虑括号与乘除号
bool Solve2(napi_env env)
{
    Solve2Init();

    string expression = "", answer = "";

    operatorNum = GetNum(1, 3);
    operandNum = operatorNum + 1;

    for (int32_t i = 0; i < operatorNum; i++)
        operators[i] = GetNum(10, 11);

    // 生成分子与分母，保证分子严格小于分母
    denominators[0] = GetNum(1, 66);
    numerators[0] = GetNum(1, max(denominators[0] - 22, 1));

    int32_t g = GCD(numerators[0], denominators[0]);
    numerators[0] /= g, denominators[0] /= g;

    int32_t nowNume = numerators[0], nowDeno = denominators[0];
    for (int32_t i = 1; i < operandNum; i++)
    {
        denominators[i] = GetNum(1, 66);
        numerators[i] = GetNum(1, max(denominators[0] - 22, 1));

        g = GCD(numerators[i], denominators[i]);
        numerators[i] /= g, denominators[i] /= g;

        g = GCD(denominators[i], nowDeno);
        int32_t lcm = nowDeno * denominators[i] / g;
        nowNume *= denominators[i] / g;

        if (operators[i - 1] == 10)
        {
            nowNume += numerators[i] * nowDeno / g;
            if (nowNume >= nowDeno)
                return false;
        }
        else if (operators[i - 1] == 11)
        {
            nowNume -= numerators[i] * nowDeno / g;
            if (nowNume <= 0)
                return false;
        }

        nowDeno = lcm;
        g = GCD(nowNume, nowDeno);
        nowNume /= g, nowDeno /= g;

        // 当运算过程中分子分母大于阈值（这里是 666 ），则重新生成算式
        if (nowNume > 666 || nowDeno > 666)
            return false;
    }
    expression += (to_string(numerators[0]) + "/" + to_string(denominators[0]));

    for (int32_t i = 1; i < operandNum; i++)
    {
        expression += op[operators[i - 1]];
        expression += (to_string(numerators[i]) + "/" + to_string(denominators[i]));
    }
    answer += (to_string(nowNume) + "/" + to_string(nowDeno));

    AddRes(env, expression.c_str(), answer.c_str());

    return true;
}

// 这里 x 与模数控制分数运算式出现的概率
// 这里限定为 30% 的分数运算式， 50% 的有括号普通运算式 ，20% 的无括号普通运算式
bool Solve(napi_env env, int32_t value_upper_limit)
{
    int32_t x = GetNum(0, value_upper_limit);

    if (x <= 2)
    {
        while (!Solve2(env))
            continue;

        return true;
    }
    else if (x <= 7)
    {
        while (!Solve1(env, true, value_upper_limit))
            continue;

        return true;
    }
    else
    {
        while (!Solve1(env, false, value_upper_limit))
            continue;

        return true;
    }
}

bool Generator(napi_env env, int32_t expressions_total, int32_t value_upper_limit)
{
    srand(time(0));
    InitMap();

    for (int32_t i = 0; i < expressions_total; i++)
    {
        if (Solve(env, value_upper_limit))
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

napi_value Addon(napi_env env, napi_callback_info info)
{
    napi_status status;

    // Check numbers of arguments

    size_t argc = 2;
    napi_value args[2];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    assert(status == napi_ok);

    if (argc < 2)
    {
        napi_throw_type_error(env, nullptr, "Wrong number of arguments");

        return nullptr;
    }

    // Type checking of arguments

    napi_valuetype arg0_type;
    status = napi_typeof(env, args[0], &arg0_type);
    assert(status == napi_ok);

    napi_valuetype arg1_type;
    status = napi_typeof(env, args[1], &arg1_type);
    assert(status == napi_ok);

    if (arg0_type != napi_number || arg1_type != napi_number)
    {
        napi_throw_type_error(env, nullptr, "Wrong types of arguments");

        return nullptr;
    }

    // Check value of arguments

    int32_t arg0; // expressionsTotal
    status = napi_get_value_int32(env, args[0], &arg0);
    assert(status == napi_ok);

    int32_t arg1; // valueUpperLimit
    status = napi_get_value_int32(env, args[1], &arg1);
    assert(status == napi_ok);

    if (arg0 <= 0 || arg1 <= 0)
    {
        napi_throw_type_error(env, nullptr, "Wrong arguments");

        return nullptr;
    }

    InitRes(env);

    if (Generator(env, arg0, arg1))
    {
        return res;
    }
    else
    {
        napi_throw_type_error(env, nullptr, "Error!");

        return nullptr;
    }
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor desc = {"generate", 0, Addon, 0, 0, 0, napi_default, 0};
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    return exports;
}

NAPI_MODULE(Addon, Init)
