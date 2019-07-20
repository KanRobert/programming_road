# 内容概要
1. 本文为[Kaleidoscope: Implementing a Parser and AST](https://llvm.org/docs/tutorial/LangImpl02.html)的读书笔记
1. 第二章介绍
1. 抽象语法树（AST）
1. 解析器（Parser）基础
1. 基本表达式解析
1. 二元表达式解析
1. 解析剩余部分
1. 驱动
1. 结论
1. 完整代码

# 参考文献

# 2. 文档介绍和词法分析器

## 2.1 第二章介绍
本章将展示如何利用第一章实现的Lexer来实现一个**Kaleidoscope**语言的解析器（Parser）。实现Parser之后，我们会构造一个抽象语法树（AST）。

Parser将会使用递归下降解析和运算符优先解析的组合来解析**Kaleidoscope**语言（解析二元表达式用后者，解析其他用前者）。

## 2.2 抽象语法树（AST）
抽象语法树捕获程序的行为，使得编译器后面的阶段（比如代码生成）更加容易。我们希望对于语言中的每一个结构都有一个AST对象，AST应该精确建模这门语言。在**Kaleidoscope**中，我们有表达式（expression），原型（prototype）和函数对象（function object）。我们从表达式先开始：
### 2.2.1 表达式语法树
```Cpp
/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  virtual ~ExprAST() {}
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  double Val;

public:
  NumberExprAST(double Val) : Val(Val) {}
};
```
上面的代码定义了基类```ExprAST```和一个为number定义的派生类```NumberExprAST```。```NumberExprAST```简单地将number捕获为自己的域。

我们再看看其他表达式的抽象语法树的定义
```Cpp
/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(const std::string &Name) : Name(Name) {}
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
    : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
};
```
定义都很简单明了，```VariableExprAST```捕获变量名，```BinaryExprAST```捕获操作符和左右操作数，```CallExprAST```捕获函数名和函数参数。

上面已经给出了所有的表达式语法树的派生类了。目前为止，**Kaleidoscope**还没有条件控制流，因此其还不是图灵完备的，后面我们会fix这一点。

### 2.2.2 原型语法树和函数语法树
```Cpp
/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> Args)
    : Name(name), Args(std::move(Args)) {}

  const std::string &getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
};
```
在**Kaleidoscope**中，因为只支持64位浮点型，因此我们不需要存储实参的类型。在稍微有点“上进心”的语言中，```ExprAST```应该还有一个指定其类型的域。

## 2.3 解析器基础
### 2.3.1 解析目标
既然要构造语法树，我们就要定义构造语法树的Parser代码。我们想要把类似"x+y"的字符串(由lexer作为3个token返回)Parse成一个语法树，这个语法树跟下面生成的语法树等价
```Cpp
auto LHS = llvm::make_unique<VariableExprAST>("x");
auto RHS = llvm::make_unique<VariableExprAST>("y");
auto Result = std::make_unique<BinaryExprAST>('+', std::move(LHS),
                                              std::move(RHS));
```
### 2.3.2 辅助函数
为了实现这个目标，我们先定义一些helper
```Cpp
/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results.
static int CurTok;
static int getNextToken() {
  return CurTok = gettok();
}
```
```getNextToken()```多读了一个token放在了```CurTol```，作为lexer的token buffer。
```Cpp
/// LogError* - These are little helper functions for error handling.
std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "LogError: %s\n", Str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}
```
```LogError```以及与它相似的函数是用来帮助我们处理error的，它们虽然一点也不user-friendly，但是足够简单。

## 2.4 基本表达式解析
我们先从number的解析开始，因为它最简单。
```Cpp
/// numberexpr ::= number
static std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto Result = llvm::make_unique<NumberExprAST>(NumVal);
  getNextToken(); // consume the number
  return std::move(Result);
}
```
这个函数很简单，当```gettok```返回的是```tok_number```时```ParseNumberExpr```会被调用，用当前number的值构造一个NumberExprAST，buffer一个token，然后返回。

