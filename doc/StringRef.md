# 内容概要


# 参考文献
1. [function alias](https://stackoverflow.com/questions/3053561/how-do-i-assign-an-alias-to-a-function-name-in-c)

```C
template <typename T>
iterator_range<decltype(adl_begin(std::declval<T>()))> drop_begin(T &&t,
                                                                  int n) {
  return make_range(std::next(adl_begin(t), n), adl_end(t));
}
```
`T`应该是一个container, `decltype(adl_begin(std::declval<T>()))`应该是其迭代器的类型。我们先看一下[`next`的作用](https://en.cppreference.com/w/cpp/iterator/next)，就是对迭代器进行`++`操作。再看一下`adl_begin`的定义

```C
 template <typename ContainerTy>
 auto adl_begin(ContainerTy &&container)
     -> decltype(begin(std::forward<ContainerTy>(container))) {
   return begin(std::forward<ContainerTy>(container));
 }
 ```
 其返回的是`forward<ContainerTy>`的迭代器类型，对比可知这里的`ContainerTy`是`declval<T>`。

 `declval<T>`的类型是`T &&`，那forward

 Lexer.cpp 
 LexTokenInternal
 Lex