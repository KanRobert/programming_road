# 内容概要
1. 本文为[Kaleidoscope: Tutorial Introduction and the Lexer](https://llvm.org/docs/tutorial/LangImpl01.html)的读书笔记
1. 文档介绍
1. 基础语言
1. 词法分析器（Lexer）

# 参考文献
[strtod 说明](https://en.cppreference.com/w/c/string/byte/strtof)

# 1. 文档介绍和词法分析器

## 1.1 文档介绍
本文档将介绍一门简单语言**Kaleidoscope**的实现，让你可以轻松开心地上手LLVM，在这里学到的知识可以拓展到其他语言。

## 1.2 要实现的语言
* 语言名称： Kaleidoscope
* 支持的操作：
    1. if/then/else 结构
    1. for循环
    1. 用户自定义运算符
    1. 使用简单的命令行接口进行JIT编译等
* 支持的数据类型： 64位浮点型（也就是C语言中的double）

下面给一个计算Fibonacci数的函数的定义和调用的例子
```sh
# Compute the x'th fibonacci number.
def fib(x)
  if x < 3 then
    1
  else
    fib(x-1)+fib(x-2)

# This expression will compute the 40th number.
fib(40)
```
**Kaleidoscope**还允许我们调用标准库函数，我们只要在使用前用```extern```关键字声明它就行
```sh
extern sin(arg);
extern cos(arg);
extern atan2(arg1 arg2);

atan2(sin(.4), cos(42))
```

## 1.3 词法分析器（Lexer）
当我们说到实现一门语言的时候，第一件要做到的事肯定就是处理一个文本文件，并且识别它说了什么。经典的方法是使用一个**Lexer**将文本输入分解成一个个**token**。Lexer将返回每个token的代码以及可能的metadata（e.g. 数字的数值）。首先，我们定义一下可能的token代码
```Cpp
// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.
enum Token {
  tok_eof = -1,

  // commands
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5,
};

static std::string IdentifierStr; // Filled in if tok_identifier
static double NumVal;             // Filled in if tok_number
```
lexer返回的token代码要么是枚举型```Token```的可取值之一，要么是一个未被识别的字符譬如说'+'（lexer将会返回其ASCII码）。如果当前的token是一个identifier，全局变量```IdentifierStr```就会存放该identifier的名字；如果当前的token是一个number（如1.0），则```NumVal```就存放了该number的数值。

我们只用一个函数```gettok```就可以实现我们的lexer，当我们调用```gettok```时，其会从标准输入返回下一个token，并且可能设置```IdentifierStr```或```NumVal```的值。我们将```gettok```的定义拆成若干部分分别阐述:

```Cpp
/// gettok - Return the next token from standard input.
static int gettok() {
    static int LastChar = ' ';
    // Skip any whitespace.
    while (isspace(LastChar))
        LastChar = getchar();
```
``` ```
```Cpp
    if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = LastChar;
        while (isalnum((LastChar = getchar())))
            IdentifierStr += LastChar;

        if (IdentifierStr == "def") return tok_def;
        if (IdentifierStr == "extern") return tok_extern;
        return tok_identifier;
    }
```
``` ```
```Cpp
    if (isdigit(LastChar) || LastChar == '.') {   // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }   
```
``` ```
```Cpp
    if (LastChar == '#') {
        // Comment until end of line.
        do
            LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF) return gettok();
    }
```
``` ```
```Cpp
    // Check for end of file.  Don't eat the EOF.
    if (LastChar == EOF) return tok_eof;

    // Otherwise, just return the character as its ascii value.
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}
```
```gettok```调用C语言的```getchar()```从标准输入读取字符，忽略空白字符，并将读到的第一个非空白字符会放在```LastChar```中。

**Kaleidoscope**的标识符以字母开头，后面跟任意个字母或数字（关键字属于特殊的标识符）。如果读到的第一个字符是字母，我们就从标准输入贪心地读取符合```[a-zA-Z][a-zA-Z0-9]*```的字符串到```IdentifierStr```中。如果```IdentifierStr```是```def```或者```extern```就返回相应的token代码，否则返回标识符对应的token代码（```tok_identifier```）。

如果读到的第一个字符是数字或者小数点，我们就从标准输入贪心地读取符合```[0-9.]+*```的字符串到```NumStr```中。```[0-9.]+*```表示数字/小数点出现一次或多次，显然这样读到的不一定是个number，譬如“1.23.45.67”，但是我们暂时忽略这个情况。接着我们调用C语言的```strtod```函数将```NumStr```转化为数值存放在```NumVal```中，转化也是贪心的，例如“1.23.45.67”会被转化为1.23。转化完成后返回number对应的token代码（```tok_number```）。

**Kaleidoscope**的注释为单行注释，以```# ```开头，以换行符结束，当前
```# ```右侧的所有内容都会被编译器忽略，这种注释可以包含任何文本，也包括额外的```# ```。如果读到的第一个字符是```# ```，我们就利用循环结构跳过当前行，只要未达到文件尾，就返回下一个token。

最后，如果读到的字符上述情况都不满足。先判断它是不是文件结束符。如果是，返回```tok_eof```，否则返回该字符的ascii码。

> Note ：在返回每个token之前（文件结束符除外），我们都多consume了一个字符。譬如在处理"a1.9 "的时候，将"a1"作为标识符返回之前，我们将'.'符号consume掉了，下次在调用```gettok()```的时候，就会cosume掉"9 "并返回一个"tok_number"。也就是说，在贪心地读取字符作为合法token时，第一个使贪心过程退出的字符充当了token间的分隔符。