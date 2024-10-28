{{ self.title() }}

{{ s('Background') }}

MapReduce 最早是由 Google 公司研究提出的一种面向大规模数据处理的并行计算模型和方法，其核心操作是 `map()` 和 `reduce()` ，概念借鉴于函数性编程语言。支持 MapReduce 的软件系统通常运行在由成百上千台服务器组成的分布式集群上，用于处理大规模数据的计算问题，例如 Google 搜索引擎中的排序、数据挖掘、机器学习等任务。

{{ s('description') }}

在本题中，我们对 MapReduce 框架进行了相当的简化，只保留了几个简单的接口函数，且不涉及分布式存储和分布式计算的问题。你需要对接口函数进行具体实现。在评测时，会对这些接口函数进行调用，完成特定功能，进行正确性和性能测试，根据测试结果给分。

本题仅提供 C++/ Java 解题框架。

此外，为了让你更方便的理解相关知识与解决这一问题，我们还提供了一些相关的学习资料供你参考，其中既包括基本原理的阐述，也包括了一些相关的研究论文等等。我们并不保证这些资料对你的实现都有帮助。

### 任务目标

本次任务的目标为实现下列的接口，我们对于 C++/Java 分别给出实现说明。这些接口都是不可变（immutable）的，即所有返回 `Stream` 的接口都应该返回新的对象，而不应该改变被调用对象的状态。在 C++ 版本中，我们给函数添加了 `const` 限制符以保证你做到这一点；在 Java 版本中，请特别注意这一规定。我们将不变性的正确实现作为运行其他测试的前置条件；如果此特性实现不正确，则你无法获得任何分数。

#### C++ 接口说明

在 `Stream.hpp` 中，给出了 Stream 类的定义及成员函数，你需要在该文件中对成员函数进行实现，函数说明如下：

#####  `Stream(const std::vector<T>& original_data)`

`Stream` 类的构造函数。传入数据是装有类型 `T` 数据的 `std::vector`，生成一个基于类型 `T` 的 `Stream` 类对象。后续的操作即基于该 `vector` 中的数据进行。我们提供了一个该函数的简单实现。

**注意：** 选手可根据需要对该函数的实现进行改动，也可在 `Stream` 类中增删成员变量和成员函数，只需保证已有的在 public 域中的函数可被调用且正确运行即可。

#####  `Stream<M> map(std::function<M(const T&)> map_func) const;`

`map` 函数的意义是将当前 `Stream` 对象基于的 `T` 数据依次变换为 M 类型的数据，并返回一个基于 `M` 类型的数据的 `Stream` 对象。`map` 函数的参数也是一个函数，它的输入是一个 `T` 类型的数据，返回一个 `M` 类型的数据，它指明了变换操作的具体实现，是由调用接口的用户来编写的，换言之，你可以不用关心 `map_func` 的具体实现。

#####  `Stream<T> filter(std::function<bool(const T&)> filter_func) const;`

`filter` 函数的意义是对当前 `Stream` 对象基于的所有 `T` 类型的数据进行过滤，它的传入参数是函数 `filter_func`，返回一个基于所有通过过滤的 `T` 数据的 `Stream` 对象。`filter_func` 函数接受一个 `T` 类型的对象，并返回一个 `bool` 值，返回值为 false 的 `T` 数据将被从返回的 `Stream` 中移除。与 `map_func` 类似，`filter_func` 也是由调用接口的用户编写的。

#####  `std::vector<std::pair<K, Stream<V>>> group_by_key(std::function<K(const T&)> get_key, std::function<V(const T&)> get_value) const;`

`group_by_key` 函数的意义是将当前 `Stream` 对应的每个 `T` 数据根据不同的 key 进行分组。每个 `T` 对象对应的 `key` （类型为 `K`）和 `value`（类型为 `V`）可以通过用户传入的 `get_key` 和 `get_value` 函数获得。而后，你需要根据 `key` 对所有 `(key, value)` 二元组进行归类，将每个 `key` 对应的所有 `value` 对象转换为一个 `Stream<V>` 对象，并将所有这样的 `std::pair<K, Stream<V>>` 对象保存在一个 `std::vector` 中返回。

注意，我们保证传入的 `K` 类型重载了下列运算符：

* `bool operator<(const K&, const K&)`  
* `bool operator==(const K&, const K&)`  
* `std::size_t std::hash<K>::operator()(const K&)`  


#####  `T reduce(T init, std::function<T(const T&, const T&)> combination_func) const;`

`reduce` 函数的意义是将当前 `Stream` 对象对应的 `T` 数据进行合并，最终得到一个 `T` 数据并返回。合并方式由传入函数 `combination_func` 决定，可以理解为这是一个二元运算，接受两个 `T` 类型的数据，返回一个。`combination_func` 是由调用接口的用户编写的。

我们保证传入的 `combination_func` 满足以下性质：

* 交换律：交换两个参数，结果不变
* 结合律：多次函数调用可以任意结合

##### `std::vector<T> collect();`

`collect` 函数的功能是，取出当前 Stream 中所有数据的值，并存储在 vector 中返回。我们不对 `vector` 中元素的顺序进行要求，但是你需要保证 `Stream` 和 `vector` 中的元素能一一对应。

#### Java 接口说明

接口的语义与 C++ 语言的基本一致，我们只对语法上的不同进行说明。因此，你应该先仔细阅读 C++ 版本的接口说明，以理解各接口的语义。

需要特别提醒的是，我们使用的 `List<T>` 是一个接口，并不能直接作为容器使用，`ArrayList` 和 `Vector` 等容器都实现了这一接口。

##### `public MyStream(List<T> data)`

构造函数。

##### `public <U> MyStream<U> map(Function<T, U> transformer)`

对每个元素进行转换。对于一个类型为 `T` 的对象 `t`，可以调用 `transformer.apply(t)` 获得对应的类型为 `U` 的结果。

##### `public MyStream<T> filter(Predicate<T> condition)`

对元素进行过滤。对于一个类型为 `T` 的对象 `t`，可以调用 `condition.test(t)` 获得一个布尔值表明它是否应该保留。

##### `public <K extends Comparable<K>, V> List<Pair<K, MyStream<V>>> groupByKey(Function<T, K> keyGetter, Function<T, V> valueGetter)`

对元素进行按 Key 分组。`Function` 对象的使用与上面类似。请注意，此时的 `Key` 类型保证重载了以下的接口：

* `bool equals(Object o)`  
* `int hashCode()`  
* `int compareTo(Key k)`  

##### `public T reduce(T initial, BinaryOperator<T> reducer)`

对数据进行合并，你可以使用 `reducer.apply(t1, t2)` 得到两个元素的运算结果。同样，我们也保证运算的交换性和结合性。

##### `public List<T> collect()`

将 `MyStream` 中的数据存储在 `List` 中返回，同样我们不对元素的顺序提出要求。

#### 测试框架

在 `main.cpp` / `Main.java` 中，提供了一个用上述接口进行单词计数任务的样例，对文本文件中出现的以字母`a`开头的词进行计数，同时提供了 `test.txt` 供你测试使用。
你可以通过这个例子来了解用户是怎么调用这些接口的，从而更好地进行实现。你也可以自己编写代码对接口进行测试。

#### 提交说明

你只需要提交 `Stream.hpp` / `MyStream.java`，评测系统会对其进行编译并调用接口运行。原则上你需要保证文件名称、需调用的类名称、所有 `public` 的函数签名与所提供的文件保持一致。你可以任意更改这两个文件，只要能够通过 OJ 的编译即可。

{{ s('scoring') }}

本题每一个测试点独立评分，共有4个测试点，分别占 10、20、30、40 分。

每个测试点的分数分为两部分：正确性分数和性能分数。

正确性分数占 $30\%$ ，评测系统会调用你实现的接口函数，若能给出正确的结果，则得到全部正确性分数，否则整个测试点得 $0$ 分。

性能分数占 $70\%$ ，以程序运行时间为基准计算，规则如下：

1. 运行时间仅包含接口函数运行时间与一些必要的其他操作，不含输入输出等时间
2. 每个分赛区单独评分，赛区用时最短者得到全部性能分。假设赛区最短用时为 $t_0$ ，则用时为 $t$ 的选手在功能正确的情况下，将得到总性能分数的 $t_0 / t$。


{{ s('hint') }}

1. 在提交的代码中禁止进行任何IO操作，否则将被判为违规，本题成绩无效。
2. 你可以使用多线程等手段来提高你的程序性能，如 pthread 和 OpenMP 等。我们提供的评测机基于虚拟化技术构建，有 4 个逻辑处理器，你可以参考这一数据。
3. 禁止对提供的代码框架进行任何形式的逆向工程或攻击，否则将被判定为违规，并取消全部成绩。
