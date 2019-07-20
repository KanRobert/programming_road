# 内容概要


# 参考文献
1. [Production rule](https://en.wikipedia.org/wiki/Production_(computer_science))
1. [Formal grammar](https://en.wikipedia.org/wiki/Formal_grammar)
1. `<<自己手动构造编译系统>>`
1. `<<编译器设计>>`
1. [为什么递归下降分析中要消除左递归？](https://www.zhihu.com/question/28397643)
1. [如何关闭gdb的layout?](https://stackoverflow.com/questions/8409540/how-to-close-layout-src-windows-in-gdb) `Ctrl+x+a`
1. `<<Engineering a Compiler>>` chapter 3

# 1. 产生式(Production)
在编译原理中, **产生式** 就是符号替换的重写规则，根据这些重写规则，我们可以由一个符号递归地产生多个符号序列。通常我们在产生式中用 **大写字母** 表示 **非终结符** (需要被继续替换的符号)，用 **小写字母** 表示 **终结符** （不需要且不能被替换的符号）。譬如，下面这条产生式中
```
S->Sa | b
```
`S`是终结符，`a`和`b`是非终结符，`|`表示既可以替换成左边地，也可以替换成右边的。因此这条产生式是说，我们可以把`S`替换成`Sa`或`b`。而`Sa`中的`S`又可以应用产生式规则替换，因此我们就可以得到`Saa`, `ba`, `b`。依此递归，我们不难看出，我可以得到的由终结符组成的符号序列有`b`, `ba`, `baa` `baaa`...。


# 2. 文法定义
Chomsky于1956年建立了形式语言的描述，他把文法分为四种类型，即0型、1型、2型、3型文法，这四种文法对产生式的规则限制依此增多，因此能表达的形式语言也越来越少。其中最重要的两个文法是上下文无关文法(即2型文法)和正则文法(即3型文法)，尽管它们的表达能力要比上下文有关文法（即1型文法）和无限制文法（0型文法）要弱，（0型文法可以表达任意可以被图灵机识别的语言），但是2、3型文法的解析器可以被高效得实现。

前面讨论的词法记号属于3型文法，使用 **确定有限状态自动机(DFA)** 就可以识别，程序设计语言通常使用的是2型文法，需要进行语法分析才能识别。

# 3. 语法分析的作用
**语法分析器(Parser)** 获取词法分析器提供的词法记号序列，根据高级语言文法的结构，构造一颗完整的 **抽象语法树(AST)** ，抽象语法树的子树（包括抽象语法树本身）也称为 **语法模块**。抽象语法树的根节点就表示语法的起始符号，抽象语法树的叶子节点，从左到右依次与词法分析器(Lexer)返回的词法记号(Token)对应。

![Parser inoutput](rc/parser-inoutput.png)
## 3.1 自顶向下的 Parser
根据构造语法树的方式，我们可以将Parser分成自顶向下的Parser和自底向上的Parser。自底向上的Parser我们不研究。在构造抽象语法树(AST)的过程中，自顶向下的Parser从根节点开始，一步步扩展AST一直到其叶子节点与Lexer返回的词法记号匹配。

# 3.2 LL(1) Parser
我们这里要实现的Parser是LL(1) Parser，**LL(1)** Parser是一种高效的自顶向下的Parser。其得名于以下事实：这种Parser由左(Left, L)向右扫描其输入，构建一个最左推导(Leftmost, L)，仅使用一个前看符号(1)。编译器GCC也是使用LL Parser完成C语言的语法分析的，不过GCC使用的是LL(2)分析算法。

# 4. LL(1) Parser对产生式的要求
LL(1) Parser要求产生式必须是**右递归**、**右侧无左公因子**。

为什么有这两个要求呢？这是因为LL(1) Parser需要在前看一个符号（词法记号）的情况下唯一确定产生式使用哪一个重写规则。如果出现左递归，譬如下面这条产生式
```
S->Sa | b
```
Parser在读入第一个符号时，会递归下降规则1的最左侧的终结符，但是显然展开了一个无限展开的死循环
```
S->Sa->Saa->Saaa->...
```
如果出现左公因子的话，会导致Parser无法做出唯一的选择。譬如说
```
S->aA | aB
```
LL(1) Parser在前看一个符号`a`后，就不知道该将`S`推导成`aA`还是`aB`。

# 5. <program>的产生式
为了方便表示和理解，我们下面将非终结符放在尖括号里，终结符直接写。

如果用非终结符`<program>`表示高级语言程序，用`<segment>`表示程序的一个的片段，那么高级语言程序与程序片段的关系如下：
```
<program>-><segment><program>
```
通过递归替换，`<program>`可以推导出任意多个`<segment>`，而高级语言程序包含的程序片段肯定是有限多个，因此我们还需要给推导过程一个终止条件。
```
<program>->ε
```
特殊终结符`ε`表示空。我们可以利用`|`将上面两个产生式合并成一条
```
<program>-><segment><program> | ε
```
在该产生式中，不断被替换的非终结符`<program>`出现在产生式的最右侧(当然我们说的是由`|`分隔的每个单元的最右侧)，因此其是右递归的，满足LL(1) Parser的要求。
# 6. 消除左递归和左公因子
## 6.1 消除左递归
你可能会发现，高级语言程序与程序片段的关系还可以这样表示
```
<program>-><program><segment> | ε
```
显然这与前面的产生式完全等价，不过这条产生式是左递归的，不满足LL(1) Parser的要求。我们可以通过改写产生式消除左递归，对于左递归产生式
```
S->Sa | b
```
可以改写成
```
S->bS'
S'->aS' | ε
```
因此`<program>-><program><segment> | ε`改写后变为
```
<program>->ε<program'>
<program'>-><segment><program'> | ε
```
产生式`<program>->ε<program'>`等价于`<program>-><program'>`，这条产生式是冗余的，可以消除。使用非终结符`<program>`代替`<program'>`，即得到产生式`<program>-><segment><program> | ε`。
# 6.2 消除左公因子
消除左公因子很简单，提出来就好了，譬如下面的产生式
```
S->aA | aB
```
可以重写成
```
S->aS'
S'->A | B
```
如果A和B还有左公因子，按上述过程继续重写即可。
# 7. `<segment>`的产生式
程序片段包含变量声明、变量定义、函数声明和函数定义。代码示例如下
```cpp
extern int name; // declare variable 'name'
int name; // define variable 'name'
int *name; // define pointer variable 'name'
int name(){} // define function 'name'
int name(); // declare function 'name'
```
可以看出，使用`extern`关键字引导的肯定是声明，紧跟`extern`之后的是数据类型，否则引导的代码可能是定义，也可能是函数声明，我们用如下产生式表示`<segment>`
```
<segment>->kw_extern <segment'> | <segment'>
<segment'>-><type><def>
<type>->kw_int | kw_char | kw_void
```
其中`<def>`表示类似`name;`, `name=0;`, `name();`, `*name=0;`, `name(){};`这样的结构。
由于`<segment'>`只有一种推导方式，因此可以将其合并到`<segment>`的产生式内。
```
<segment>->kw_extern <type><def> | <type><def>
<type>->kw_int | kw_char | kw_void
```
不难注意到，根据`<segment>`的产生式定义,如下代码也会被识别成三个`<segment>`
```cpp
extern int name =0;
extern int name(){}
extern int name(); 
```
我们使用`extern`关键字是希望其后的变量在其他编译单元中定义，函数的声明和定义不需要加`extern`。因此我们不希望上面三行代码被识别成合法的`segment`。虽然我们可以通过不同的`<def>`形式来完成这种区分，但是这样做会让文法显得更加复杂。对于这种情况，可以将这种合法性检查推迟到语义分析时进行。语义分析时，针对带有`extern`关键字的变量定义、函数声明**在语法分析过程中难以或者无法处理的问题，可以由语义分析来辅助解决**。

# 8. `<def>`的产生式
以下是我们支持的符合`<def>`定义的代码(括号中的符号是属于`<type>`，写在这里是为了让`<def>`的定义更清晰)。
```cpp
// 单个变量或数组或指针
(int) name;
(int) name=1
(int) name=a+b;
(int) arr[10];
(int) *ptr;
```
```cpp
// 单个变量`name` + 变量列表`,name2, name3;`
(int) name, name2, name3;
```
```cpp
// 函数
(void) fun();
(void) fun(){}
(int) fun(int x, char *y)
```
可以看到，满足`<def>`的定义的代码结构非常多，所以我们要进行合理的划分。我们将`def`分为如上所示的三大类。我们允许变量和指针使用表达式初始化，为了简化起见，不允许对数组初始化。
## 8.1 单个变量或数组或指针`<defdata>`
不包含末尾的`;`，我们用`<defdata>`表示单个变量或数组或指针。其产生式可以这么写
```
<defdata>->id <vardef> | mul id <init>
<vardef>->lbrack num rbrack | <init>
<init>->assign <expr> | ε
```
非终结符`<vardef>`表示类似`name;`, `arr[10]`的结构，`<init>`表示初始化部分，`<expr>`表示表达式。
## 8.2 变量列表`<deflist>`
我们用`<deflist>`表示形如`,name2, name3;`的变量列表，其产生式为
```cpp
<deflist>->comma <defdata><deflist> | semicon
```
这里我们使用右递归`<deflist>->comma <defdata><deflist>`表示`<deflist>`可以由任意多个`comma <defdata>`组合，并使用分号`semicon`右递归的无限推导过程。

这样一来，我们就可以使用`<defdata><deflist>`表示所有的变量定义结构。
## 8.3 函数`<fun>`
我们用`<fun>`表示除去返回类型的函数声明或定义
```
<fun>->id lparen <para> rparen <funtail>
<funtail>->semicon | <block>
```
非终结符`<para>`表示形参列表，`<block>`表示函数体。
## 8.4 合体
因此，`<def>`的产生式我们可以表示为
```
<def>-><defdata><deflist> | <fun>
```
但是这个产生式式不满足LL(1)文法要求的，因为非终结符`<defdata>`和`<fun>`有共同的左公因子`id`！为了解决这个问题，我们需要将`<def>`展开
```
<def>->id <vardef><deflist>
     | mul id <init><deflist>
     | id lparen <para> rparen <funtail>
```
合并一下左公因子，我们就可以得到
```
<def>->id <idtail> | mul id <init><deflist>
<idtail>-><vardef><deflist> | lparen <para> rparen <funtail>
<funtail>->semicon | <block>
```
根据`<def>`的定义，我们支持的函数的返回类型只能是基本类型，而不能是指针类型。另外文法定义中允许出现`void`类型的变量，这个问题也留到语义分析时再解决。

# 9. 形参列表`<para>`的产生式
为了简化文法的构造，我们对形参的定义作如下限制
1. 形参名称不能省略
1. 形参不允许有默认值
1. 数组参数必须指定数组长度

## 9.1 单个形参去除类型的部分`<paradata>`
我们使用`<paradata>`表示单个形参去除类型的部分，其产生式为
```
<paradata>->mul id | id <paradatatail>
<paradatatail>->lbrack num rbrack | ε
```
于是，我们可以使用非终结符组合`<type><paradata>`表示一个完整的形参
## 9.2 去除第一个形参的形参列表`<paralist>`
去除第一个形参的形参列表`<paralist>`与变量列表`<deflist>`的定义很像
```
<paralist>->comma <type><paradata><paralist> | ε
```
使用右递归表示可以由任意个`comma <type><paradata>`组成。
## 9.3 合体
因此我们可以使用非终结符组合`<type<paradata><paralist>`来表示一个完整的非空形参列表，`<para>`的产生式可以表示为
```
<para>-><type><paradata><paralist> | ε
```
# 10. 函数体`<block>`的产生式
函数体是由花括号包含起来的局部变量定义或语句的组合，因此`<block>`的产生式如下
```
<block>->lbrace <subprogram> rbrace
<subprogram>-><localdef><subprogram>
            | <statement><subprogram>
            | ε
```
非终结符`<subprogram>`表示子程序的内容，`<localdef>`表示局部变量定义，`<statement>`表示语句。值得一提的是，由于`<localdef>`都是以类型词法记号引导的，而`<statement>`不存在以类型词法记号开头的情况，因此二者是没有左公因子的。
## 10.1 局部变量`<localdef>`的产生式
局部变量与前面描述的全局变量的定义完全相同，其产生式为
```
<localdef>-><type><defdata><deflist>
```
# 11. 语句`<statement>`的产生式
## 11.1 表达式`<expr>`的产生式
赋值表达式`<assexpr>`
```
<assexpr>-><orexpr><asstail>
<asstail>->assign <orexpr><asstail> | ε
```
逻辑或表达式`<orexpr>`
```
<orexpr>-><andexpr><ortail>
<ortail>->or <andexpr><ortail> | ε
```
逻辑与表达式`<andexpr>`
```
<andexpr>-><cmpexpr><andtail>
<andtail>->and <cmpexpr><andtail> | ε
```
关系表达式`<cmpexpr>`
```
<cmpexpr>-><addsubexpr><cmptail>
<cmptail>-><cmps><addsubexpr><cmptail> | ε
<cmps>->gt | ge | lt | le | equ | nequ 
```
加减法运算表达式`<addsubexpr>`
```
<addsubexpr>-><muldivmodexpr><addsubtail> 
<addsubtail>-><addsub><muldivmodexpr><addsubtail> | ε
<addsub>->add | sub
```
乘除求余运算表达式`<muldivmodexpr>`
```
<muldivmodexpr>-><unaryexpr><muldivmodetail>
<muldivmodetail>-><muldivmode><unaryexpr><muldivmodetail>
<muldivmode>-> mul | div | mod
```
单目运算表达式`<unaryexpr>`
```
<unaryexpr>-><unary><unaryexpr> | <val>
<unary>->not | sub | lea | mul | incr | decr
```
值表达式`<val>`
```
<val>-><elem><incrdecr>
<incrdecr>->incr | decr
```
元素表达式`<elem>`, 包含变量、数组、函数调用、小括号和常量。
```
<elem>->ID
      | ID LBRACK <expr> RBRACK
      | ID LPAREN <realarg> 
```