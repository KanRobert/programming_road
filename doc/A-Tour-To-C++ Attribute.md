# 内容概要
1. 什么是attribute
1. C++11到C++17的常用attribute介绍
1. 为名空间和枚举型添加attribute
1. C++17新特性：忽略未知的attribute
1. 使用using指令避免名空间的重复书写
1. Bjarne Stroustrup对attribute的使用建议

# 参考文献
1. [Attribute Syntax - Using the GNU Compiler Collection (GCC)](https://gcc.gnu.org/onlinedocs/gcc/Attribute-Syntax.html)
1. [Using the GNU Compiler Collection (GCC): Common Function Attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
1. [__declspec | Microsoft Docs](https://docs.microsoft.com/pl-pl/cpp/cpp/declspec)
1. [Attributes in Clang — Clang 5 documentation](https://clang.llvm.org/docs/AttributeReference.html)
1. [C++17 attribute](https://www.bfilipek.com/2017/07/cpp17-in-details-attributes.html)
1. [cppreference : attribute](https://en.cppreference.com/w/cpp/language/attributes)
1. [modern c attribute](https://arne-mertz.de/2016/12/modern-c-features-attributes/)
1. [common attribute intro](https://kheresy.wordpress.com/2018/07/04/c-attribute)
1. [godbolt](https://www.godbolt.org)
1. [C++11 FAQ](http://stroustrup.com/C++11FAQ.html#attributes)
1. [C++ Memory Order 与 Atomic 学习小记](https://zhuanlan.zhihu.com/p/31386431)
1. [atomic/compare_exchange](https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange)
1. [使用C++原子量实现自旋锁](https://www.cnblogs.com/FateTHarlaown/p/9170474.html)

# 1. 什么是attribute?
attribute是在基本程序代码中加入的辅助信息，编译器可以利用这些辅助信息来帮助自己CG（code generation），譬如用来优化或者产生特定代码（DLL，OpenMP等）。相比于其他语言（e.g. C#)，C++中的Meta information是由编译器决定的，你没有办法添加自己制定的attributes。而在C#中，我们可以从```System.Attribute```派生出来。

# 2. 一些你可能面熟的老朋友
在使用gcc作为compiler的代码中，经常出现的attribute
```Cpp
struct S { short f[3]; } __attribute__ ((aligned (8)));

void fatal () __attribute__ ((noreturn));
```
MSCV中import/export DLL时出现出现的attribute
```Cpp
#if COMPILING_DLL
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT __declspec(dllimport)
#endif
```
# 3. Modern C++是如何处理attribute的？
1. [Attribute Syntax - Using the GNU Compiler Collection (GCC)](https://gcc.gnu.org/onlinedocs/gcc/Attribute-Syntax.html)
1. [Using the GNU Compiler Collection (GCC): Common Function Attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
1. [__declspec | Microsoft Docs](https://docs.microsoft.com/pl-pl/cpp/cpp/declspec)
1. [Attributes in Clang — Clang 5 documentation](https://clang.llvm.org/docs/AttributeReference.html)

在C++11之前，各个编译器对attribute的写法有着自己的一套规则，很混乱。自C++11开始，我们有了越来越多的标准attribute，也就是说对于attribute有着统一的写法，而不用使用编译器特定的标记了。
# 4. 常用attribute介绍
使用attribute的基本语法是```[attribute]```和```[[namespace::attr]]```,你几乎可以在任何东西上使用attribute，例如类型、函数、对象等。
## 4.1 ```[[noreturn]]```
```[[noreturn]]```只能用于函数声明，其告诉编译器这个函数不会返回，也就是说，这个函数要么抛出异常，要么在所有执行路径上调用类似```exit```、```terminate```这些不会返回的函数。```[[noreturn]]```可以让编译器在CG的时候进行优化，也可以用来抑制编译器的warning。
```Cpp
void fatal_error() {
	throw "error";
}
 
int sw( int v) {
	switch (v) {
	case 1:
		return 0;
	default:
		fatal_error();
	}
}
```
上面的程序中，函数```sw```在```switch```语句的```default```的分支没有返回值，所以编译器会出现以下警告
```
In function 'int sw(int)':
warning: control reaches end of non-void function [-Wreturn-type]
```
但是只要```fatal_error()```改成
```Cpp
[[noreturn]] void fatal_error() {
	throw "error";
}
```
警告就消失了。

## 4.2 ```[[nodiscard]]```
如果函数被声明为```[[nodiscard]]```，或者函数by value地返回了一个被声明为```[[nodiscard]]```的enumeration/class的对象。那么，当这个函数的返回值被抛弃时，编译器会给出警告。
```Cpp
struct [[nodiscard]] error_info {
    int id;
    char *message;
};
error_info enable_missile_safety_mode();
void launch_missiles();
void test_missiles() {
   enable_missile_safety_mode(); //编译器在舍弃 nodiscard 值时发布警告
   launch_missiles();
}
```
假设把```error_info enable_missile_safety_mode();```改成```error_info& enable_missile_safety_mode();```,函数就不是按值返回的了，便不会发出警告。

```[[nodiscard]]```可以提醒使用函数的人，一定要接住这个返回值（比如说```operator new```的返回值），或者一定要检查这个返回值（比如判断是否执行成功）。

## 4.3 ```[[deprecated]]/[[deprecated("reason")]]```
```[[deprecated]]/[[deprecated("reason")]]```是告诉编译器和developer，这个东西已经被废弃了，虽然还可以用，但是最好不要用。比较常见的情况是版本更新后，整个一大块代码会被抽掉。
```Cpp
[[deprecated("Will remove in next release")]] void test() {}
void run{
    test();
}
```
使用上面的代码的时候，编译器就会给出警告
```
In function 'void run()':
warning: 'void test()' is deprecated: Will remove in next release [-Wdeprecated-declarations]
```

## 4.3 ```[[fallthrough]]```
```Cpp
switch(something) {
    case SPECIAL_CASE:
        prepareForSpecialCase();
        [[fallthrough]]
    default:
        handleAllCases();
}
```
```[[fallthrough]]```放在一个```case```块的末尾，表明你是故意没写```break```语句。不然的话编译器可以给你一个warning，static analyzer和code reviewer也会警告你。

## 4.3 ```[[maybe_unused]]```
```Cpp
static void impl1() { ... } // Compilers may warn about this
[[maybe_unused]] static void impl2() { ... } // Warning suppressed

void foo() {
   int x = 42; // Compilers may warn about this
   [[maybe_unused]] int y = 42; // Warning suppressed
}
```
有时候你声明/定义了一个函数或者变量暂时还没有用到，编译器就会warning，你可以```[[maybe_unused]]```来supress这个warning。

# 5. 为名空间和枚举型添加attribute
```Cpp
namespace [[deprecated]] old_stuff{
    void legacy();
}

old_stuff::legacy(); // Emits warning
```
我们也可以为名空间增加attribute，例如添加```[[deprecated]]```使得名空间中的所有function，type等都处于```[[deprecated]]```状态。

```Cpp
enum Algorithm {
  FPM = 0,
  FP [[deprecated]] = FPM
};

Algorithm e = FP; // Emits warning
```

假设我们定义了一个```Algorithm```的枚举类型，其中一个可选值最初为```FP=0```，但后面想用```FPM```代替```FP```这个名字，就可以像上面这个代码这样改。

# 6. 忽略未知的属性
在C++17之前你可以使用一些编译器特定的attribute，但是当你用另外一个不支持这些attribute的编译器去编译你的代码的时候，你就会得到error。但是现在若你的编译器支持C++17,它就只会报一个warning或者都不报。
```Cpp
// compilers which don't 
// support MyCompilerSpecificNamespace will ignore this attribute
[[MyCompilerSpecificNamespace::do_special_thing]] 
void foo();
```
例如，GCC 8.3 就只会报这样一个warning
```
warning: 'MyCompilerSpecificNamespace::do_special_thing' scoped attribute directive ignored [-Wattributes]
void foo();
```

# 7. 使用using指令避免名空间的重复书写
```Cpp
void f() {
    [[rpr::kernel, rpr::target(cpu,gpu)]] // repetition
    do-task();
}
```
当你使用多个同一名空间中的属性时，可以using指令简化代码
```Cpp
void f() {
    [[using rpr: kernel, target(cpu,gpu)]]
    do-task();
}
```

# 8. Bjarne Stroustrup对attribute的使用建议
```
There is a reasonable fear that attributes will be used to create language dialects. The recommendation is to use attributes to only control things that do not affect the meaning of a program but might help detect errors (e.g. [[noreturn]]) or help optimizers (e.g. [[carries_dependency]])
```

# 9. [[carries_dependency]]
```[[carries_dependency]]```涉及的知识点稍微多一点，所以我们放在最后讲。
## 9.1 功能
在函数的输入和输出中引入依赖关系，这就允许编译器skip掉不必要的memory fence指令。```[[carries_dependency]]```可以在两个情形中出现

1. 用于函数或lambda表达式的形参声明，为函数的形参初始化引入依赖。
1. 用于函数声明整体，为函数的返回值的调用引入依赖。
## 9.2 什么时release-consume模型？
release-consume模型其实就release和consume模型的组合（一共有六个内存模型，参见[memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)）。为了理解```memory_order```，我们需要理解另外两个概念**Happens-Before**和**Synchronizes-With**。

### Happens-Before 关系
这是现代编程语言中很常见的一个术语，每个语言在它们的 specifications 中都会有这个关系的定义和描述。可以这样来简单阐释这个词：
> A、B 是两个在多核 CPU 上执行的操作。如果 A happens-before B，那么 A 所产生的内存变化会在 B 操作执行之前被看到（visible）。

不管我们使用什么编程语言，在同一个线程下的顺序语句总是遵循 happens-before 原则的。就像下面代码所示：
```Cpp
int a, b;

void foo() {
    a = 42;
    b = a;

    assert(b == 42);
}
```
在单线程的情况下，断言是永远不会 fire 的。但这并不代表 b 在内存中后于 a 被修改。就像下面这段伪代码所示的：
```as
mov 42, %eax
mov %eax, (b)
mov %eax, (a)
```
虽然这并不能说明编译器就是这么处理的，但足以说明程序语义上的 happens-before 不能代表操作是真的 happened before 了。

### Synchronizes-With 关系
这个术语简单来说就是描述了一种方法，能让内存的变化（即便是非原子操作）保证被其他线程看到，这也是实现 lock-free 编程模式的基础。用一段代码来举例：
```Cpp
int data;
std::atomic_bool flag { false };

// Execute in thread A
void producer() {
    data = 42;  // (1)
    flag.store(true);  // (2)
}

// Execute in thread B
void consume() {
    while (!flag.load());  // (3)
    assert(data == 42);  // (4)
}
```
让两个函数分别在两个线程中运行，(4) 所示的断言将有一定几率 fire，这是为什么呢？虽然我们使用原子量 flag 作为“同步信号”，而且同一个线程中 happens-before 原则也一定会被遵循，但我们并不能保证 (4) 执行时 (1) 的修改一定会被 B 线程看到。这是由于现代处理器对于程序可能会采取指令重排来提高运行效率，CPU 的读写 Cache 也可能并没有写回内存。

所以，企图在多线程环境中通过某原子量来做非原子量的 Synchronization 并不是可靠的（当没有 Memory Order 的约束）。

当然，上面的代码在你的机器上可能也不会 fire，因为 C++ 默认使用 memory_order_seq_cst的内存模型。
> memory_order_seq_cst :  \
> A load operation with this memory order performs an acquire operation, a store performs a release operation, and read-modify-write performs both an acquire operation and a release operation, plus a single total order exists in which all threads observe all modifications in the same order

### Release/Acquire 操作
简单来说，一个 Release 操作会阻止当前线程中其他的读写操作被重排到这个操作之后（当然了，前提是你的代码顺序也是这样）。而 Acquire 操作会阻止当前线程中其他的读写操作被重排到这个操作之前。
这里我们改造一下上面的例子
```Cpp
int data;
std::atomic_bool flag { false };

// Execute in thread A
void producer() {
    data = 42;  // (1)
    flag.store(true, std::memory_order_release);  // (2)
}

// Execute in thread B
void consume() {
    while (!flag.load(std::memory_order_acquire));  // (3)
    assert(data == 42);  // (4)
}
```
经过改造后，(1) 不可能会被重排到 (2) 之后，也就是说，当 (3) 观察到 flag 的变化时，对于 B 线程的视角，(1) 一定已经发生了，所以 (4) 就永远不会 fire 了。

再举一个例子
```Cpp
class SpinLock {
public:
    SpinLock:flag_(false){}
    void lock() {
        bool expected = false;
        while (!flag_.compare_exchange_weak(expected, true, std::memory_order_acquire, std::memory_order_relaxed)) {
            //这里一定要将expect复原，因为在compare_exchange_weak
            //返回false的时候，expected的值是不确定的
            expected = false;
        }
    }

    void unlock() {
        flag_.store(false, std::memory_order_release);
    }
private:
    std::atomic_bool flag_;
};
```
上面的类利用CAS（Compare and Swap）操作实现了一个简单的自旋锁，```flag_```表示是否可以进入临界区。作为```atomic_bool```的成员函数，```compare_exchange_weak```在```flag_```与```expected```相等，将```flag_```置为```true```，并返回```true```，否则返回```false```。

这个例子中获得锁操作使用 acquire 的原因是为了避免 lock() 方法执行之后的语句被重排到它的上面，释放锁的操作使用 release 也是同理，避免它之前的语句被重排到它下面，不然就会出现数据访改逃逸临界区的问题了。

### Consume 操作
Consume操作通常与Release配对使用，它与Acquire操作类似，但它之限制有依赖的变量操作，阻止当前线程中对这个操作有依赖的变量操作被重排到这个操作之前。什么是有依赖的变量操作？cppreference说得很清楚
>Within the same thread, evaluation A that is sequenced-before evaluation B may also carry a dependency into B (that is, B depends on A), if any of the following is true 
>1. The value of A is used as an operand of B, except \
    >a. if B is a call to std::```kill_dependency``` \
    >b. if A is the left operand of the built-in ```&&, ||, ?:```, or ```,``` operators. 
>2. A writes to a scalar object M, B reads from M 
>3. A carries dependency into another evaluation X, and X carries dependency into B


