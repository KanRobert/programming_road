# 内容概要
1. 本文为[Kaleidoscope: Tutorial Introduction and the Lexer](https://llvm.org/docs/tutorial/LangImpl01.html)的读书笔记
1. 文档介绍
1. 基础语言
1. 词法分析器（Lexer）

# 参考文献

# 1. 文档介绍和词法分析器

## 1.1 文档介绍
本文档将介绍一门简单语言**Kaleidoscope**的实现，让你可以轻松开心地上手LLVM，在这里学到的知识可以拓展到其他语言。

## 1.2 基础语言
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
1 // gettok - Return the next token from standard input.
2 static int gettok() {
3     static int LastChar = ' ';
4 
5     // Skip any whitespace.
6     while (isspace(LastChar))
7         LastChar = getchar();
```
```Cpp
8     if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
9         IdentifierStr = LastChar;
10         while (isalnum((LastChar = getchar())))
11             IdentifierStr += LastChar;
12 
13         if (IdentifierStr == "def") return tok_def;
14         if (IdentifierStr == "extern") return tok_extern;
15         return tok_identifier;
16     }                                                                      
17     if (isdigit(LastChar) || LastChar == '.') {   // Number: [0-9.]+
18         std::string NumStr;
19         do {
20             NumStr += LastChar;
21             LastChar = getchar();
22         } while (isdigit(LastChar) || LastChar == '.');
23 
24         NumVal = strtod(NumStr.c_str(), 0);
25         return tok_number;
26     }   
27     if (LastChar == '#') {
28         // Comment until end of line.
29         do
30             LastChar = getchar();
31         while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
32 
33         if (LastChar != EOF) return gettok();
34     }
35       // Check for end of file.  Don't eat the EOF.
36     if (LastChar == EOF) return tok_eof;
37 
38     // Otherwise, just return the character as its ascii value.
39     int ThisChar = LastChar;
40     LastChar = getchar();      
41     return ThisChar;
42 }                         
```
```gettok```调用C语言的```getchar()```从标准输入每次读取一个字符。读到的字符会放在```LastChar```中