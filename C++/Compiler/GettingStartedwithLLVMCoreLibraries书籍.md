# Getting Started with LLVM Core Libraries
[参考1](https://www.cnblogs.com/pocean/p/4021129.html)

LLVM是构架编译器(compiler)的框架系统，以C++编写而成，用于优化以任意程序语言编写的程序的编译时间(compile-time)、链接时间(link-time)、运行时间(run-time)以及空闲时间(idle-time)，对开发者保持开放，并兼容已有脚本。


## 工具和设计

追溯llvm的历史要追溯到10年前（2014），起初它着眼于编译器的后端算法和中间表述（今天依然是llvm的重点），当时其他部分还要依赖gcc。其中最重要的是它的IR，llvm在整个编译的过程中有很多种中间表示，如AST(C/C++转换成IR的过程)、DAG(IR转换成特定机器的汇编语言的过程)、某种特定的中间表示(汇编器和链接器使用)等其他中间表示依然起到很重要的作用，但是，真正让llvm与众不同的还是它的IR，甚至llvm这个名字的由来还和IR有很大的关系。

> 首先看IR(intermediate representation)的特点：

    SSA  signal static assignment 静态单赋值   三地址码
    无限多个寄存器
    
  其存在方式有两种，一种在内存中，一种是在磁盘上，SSA能够更方便的优化，再加上保存在磁盘上的这种编码方式能够达到几近全时优化的效果，这也是llvm的作者在最早介绍llvm的论文中提出的目标，但是IR的描述能力毕竟有限，要想实现全时优化，就要求对诸如程序分布这种行为做出描述，这也意味着实现了此功能的llvm要像jvm那样能够成为一个运行平台，类似jvm的名字，llvm这个名字就是这么来的。

  随着llvm项目的发展，其设计目标已经重点放在了通过磁盘上的IR操作实现链接时优化等功能，而不是全时优化，最终只保留了llvm这个名字，但是llvm已经成了一个强大的编译器框架，而不是一个类似jvm的那样的平台。

  说到这，说一下个人理解吧，感觉是llvm起初提出了一个比较精妙的编码表示方案，有利于一些编译优化，进而发现可以憧憬的的目标——全时优化，但是又发现实现全时优化比较困难，在llvm的mail list上有人专门就此发起了一个很详细的讨论，详细阐述了llvm不能像java那样成为一个platform或者vm的原因，重点还是要做编译器。
  
不管怎样，llvm这么名字还是保留下来了，现在如果有人跟你提起“LLVM”，可能是指以下内容：

    整个LLVM Project/框架
    由LLVM框架构造的编译器
    LLVM框架的可重用代码：库
    LLVM IR

  LLVM框架主要包括三个方面的核心内容：

    前端(frontend):将源码转换成中间代码(IR),包括词法分析、语法分析、语义分析、中间代码生成

    IR：中间表示 两种方式，readable和binary-encoded

    后端(backend):将IR转换成汇编代码，包括寄存器分配

  每个步骤中间交互有两种介质：1) in-memory; 2) by-file，前者变现出来通过一些高层的工具如clang，自动进行指定的动作；后者可以每一步每一步分离开来，通过一个个独立的工具完成。
  
  前端可以使用不同的编译工具对代码文件做词法分析以形成抽象语法树AST，然后将分析好的代码转换成LLVM的中间表示IR（intermediate representation）；中间部分的优化器只对中间表示IR操作，通过一系列的pass对IR做优化；后端负责将优化好的IR解释成对应平台的机器码。LLVM的优点在于，中间表示IR代码编写良好，而且不同的前端语言最终都转换成同一种的IR。

为什么使用三段式设计？优势在哪里？首先解决一个很大的问题：假如有N种语言（C、OC、C++、Swift...）的前端，同时也有M个架构（模拟器、arm64、x86...）的target，是否就需要N*M个编译器？三段式架构的价值就提现出来了，通过共享优化器的中转，很好的解决了这个问题。

> LLVM编译一个源文件的过程：

预处理 -> 词法分析 -> Token -> 语法分析 -> AST -> 代码生成 -> LLVM IR -> 优化 -> 生成汇编代码 -> Link -> 目标文件

完全需要我们手工，或者依靠其他工具如lex, yacc来做的事情，是从源代码到token的词法分析和从token到AST的语法分析；词法分析的输出是将源代码解析成一个个的token。这些token就是有类型和值的一些小单元，比如是关键字，还是数字，还是标识符，从AST转LLVM开始，LLVM就开始提供一系列的工具帮助我们快速开发。从IR(中间指令代码)到DAG(有向无环图)再到机器指令，针对常用的平台，LLVM有完善的后端。也就是说，我们只要完成了到IR这一步，后面的工作我们就享有和Clang一样的先进生产力了。

CodeGen负责将语法树从顶至下遍历，翻译成LLVM IR，LLVM IR是Frontend的输出，也是LLVM Backerend的输入，桥接前后端。
  
  
[笔记](https://blog.csdn.net/cteng/article/details/48005679)

## External Projects 附加工具

    Clang extra tools 前段附加工具
    Compiler-RT       编译器运行时
    DragonEgg         利用GCC(不仅支持类c语言解析) 得到 IR
    LLVM test suite   测试
    LLDB              类 gdb 调试器
    libc++            底层运行时库 系统动态链接库
  
## Frontend  前段  Clang
    1. 使用 libclang
    2. 理解Clang diagnostics
    3. the frontend phases：
       词法分析 Lexical analysis
       语法分析 Syntactic analysis（AST） 抽象语法树
               TranslationUnitDecl 翻译单元
               TypedefDecl   类型定义
               FunctionDecl  函数定义
               CFG
       语义分析 Semantic analysis

[编译器架构的王者LLVM——（4）简单的词法和语法分析 使用 Lex和Yacc](https://blog.csdn.net/xfxyy_sxfancy/article/details/49699519)

> 词法分析:

在正则表达式已经成为基本技能的今天，词法分析完全无门槛啊。正常情况下，我们只要写一组正则表达式，或者写个简单的状态机就可以了。

词法分析的输出是将源代码解析成一个个的token。这些token就是有类型和值的一些小单元，比如是关键字，还是数字，还是标识符等等。这个阶段不用管它们是如何组合的，都是干嘛的。

比如一个token类型是数值，值是3. 这个信息就已经足够了，至于这个3干嘛用，后面整理AST的时候再放到合适的位置上去。

至于什么时上下文无关语言，什么是确定有穷自动机，非确定有穷自动机等等这些，暂时都不需要了解。

> 语法分析

语法分析诚然是比词法分析要复杂一些。但是幸运的是，对于绝大多数语句和表达式来讲，并不需要高深的知识，“移进-归约”是个好方法，但是在我们学习的相当长的一段时期内都用不上。

语法分析的输出是抽象语法树AST，既然是棵树，自然构造时需要递归。所以在大部分的语句中，我们只按递归下降的方法就足够了。

对于表达式，递归下降还不够用，至少运算符还有优先级啊。所以针对表达式，我们还需要运算符优先分析法。SLR，LALR和LR暂时还用不上。


### ASTMatcher   抽象语法树匹配器   查找全局变量及其调用函数

[参考](https://blog.csdn.net/whdAlive/article/details/80292777?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task)

ASTMatcher 是 Clang 中用来帮助我们实现 code-to-code 的转译或者完成某些查询的工具。

 ASTMatcher提供了一个领域特定语言（DSL）来创建基于Clang AST的谓词，同时支持C++，这意味着允许用户编写一个程序来匹配AST节点并能通过访问节点的c++接口来获取该AST节点的属性、源位置等任何信息。

其主要由宏与模板驱动，用法和 函数式编程 及其类似。

ASTMatcher用来匹配AST的节点，它通过调用构造函数创建，也可以构建成一个ASTMatchers的树，其内部可以嵌套多个ASTMatcher，使得匹配更加具体准确。

所有匹配器都是名词描述实体并且可以绑定，这样它们就会指向匹配到的内容。为此，只需要在这些匹配器中调用 bind() 方法，例

```c
variable(hasType(isInteger())).bind("intvar");
```
由于Clang AST中有上千个class，我们显然不可能一个个去看去分析。

这时候我们要清楚一点：使用ASTMatcher的前提是了解你想匹配的AST的样子。

通常情况下，创建合适的ASTMatcher的策略如下：

    1. 寻找想匹配的节点的最外层的类
    2. 在 AST Matcher Reference 中查看所写的Matcher要么匹配到需要的节点，要么进行”细化”处理
    3. 创建外部匹配表达式，验证它是否按预期运行。
    4. 为接下来你想匹配的内部节点检查匹配器。
    5. 重复以上步骤,直到完成匹配器。

示例代码：
```c
#include<stdio.h>
int a;
int main(){
    a = 1;
    return 0;   
}
```
clang 编译得到 AST

clang -cc1 -ast-dump

```
|-VarDecl 0xf4a0b8 <F:\1.c:2:1, col:5> col:5 used a 'int'
`-FunctionDecl 0xf4a160 <line:3:1, line:6:1> line:3:5 main 'int ()'
  `-CompoundStmt 0xf4a248 <col:11, line:6:1>
    |-BinaryOperator 0xf4a200 <line:4:1, col:5> 'int' '='
    | |-DeclRefExpr 0xf4a1c8 <col:1> 'int' lvalue Var 0xf4a0b8 'a' 'int'
    | `-IntegerLiteral 0xf4a1e0 <col:5> 'int' 1
    `-ReturnStmt 0xf4a238 <line:5:1, col:8>
      `-IntegerLiteral 0xf4a218 <col:8> 'int' 0
```
我们可以清楚的看到，全局变量a对应的节点类型为 VarDecl ，引用该变量处的节点类型为 DeclRefExpr ，而DeclRefExpr 最外层有一层函数，对应 FunctionDecl节点类型。

得到这些信息，我们就可以总结出来”匹配模型”。

对于使用了的全局变量，我们找它的引用，这个引用需要对应于一个全局变量声明，而且引用是在某个函数内部。这种模式下，我们即可得到所有已使用了的全局变量的信息，包括在哪个函数内部调用。

转换到AST 节点来看：它首先是一个DeclRefExpr类型节点，同时它对应于一个VarDecl全局节点，而且这个DeclRefExpr节点在某个FunctionnDecl下。

因此，我们写出如下的Matcher：
```C
StatementMatcher GlobalVarMatcher = declRefExpr(
    to(
        varDecl(
            hasGlobalStorage()
        ).bind("gvarName")     // 变量引用declRefExpr --> 全局变量varDecl 节点绑定为 “gvarName”
    ) // to
    , hasAncestor(
        functionDecl().bind("function")  // 该节点其外层有函数定义 则绑定到 ”function”
    )
).bind("globalReference");               // declRefExpr节点绑定到字符串”globalReference”

```
在上述Matcher中，为匹配特定AST节点，我们把匹配的varDecl节点绑定到字符串“gvarName”，functionDecl节点绑定到字符串”function”，declRefExpr节点绑定到字符串”globalReference”，以便稍后在匹配回调中检索。

> 获取匹配节点

定义了matcher后将需要添加更多的工具来运行它们。Matchers与MatchCallback配对，并注册一个MatchFinder对象，然后从一个ClangTool运行。


## LLVM IR  中间表达
    1. 语法syntax
        a. Module: Module类声明了一个迭代器，可以遍历module中的function
        b. Function:
          Function对象可能是函数声明，可能是函数定义，获取这两个不同对象的内容可调用不同的函数。
          使用isDeclaration()方法check对象是否是函数声明，包含函数原型.
          通过getArgumentList()方法或是用arg_iterator迭代器遍历可获得函数参数。
          函数定义 遍历函数内容
             (Function::iterator i = function. begin(), e = function.end(); i != e; ++i) 
        c. BasicBlock 基本块 Basic Block类
          BB中包含了一系列指令，可以通过getTerminator()方法获取最后一条指令。
          以通过CFG，例如通过getSinglePredecessor()方法，访问前一个BB。
          
        d. Instruction 指令
          它的确切功能可以通过getOpcode（）来获取，它返回代表LLVM IR操作码的llvm::Instruction枚举的成员。
          也可以通过op_begin()和op_end()来获取操作数，这两个方法继承自User类。
          
          3层概念：Function::iterator --> BasicBlock::iterator --> i->getOpcodeName()
        e. use-def与def-use链
          【很重要的两个类——Value类和User类】
          个继承了Value类的类意味着类内定义了可能被其他类使用的结果，一个继承了User类的类意味着这个实体可能使用了一个或多个Value接口。
            Value
             Value类定义了use_begin()和use_end()方法，用于遍历Users，访问def-use链，即访问所有使用了value的user。
            
            User
           
    2. 生成器 generator
    3. IR级优化：pass
        -Ox
        -print-stats
        理解pass间依赖：
            e.g. Loop Info and Dominator Tree
        理解pass API
        定制pass
        
        优化pass管理器 LLVMPassManagerRef PM: unwrap(PM)->add(pass);
        
        
        The ImmutablePass class

Pass类的函数getAnalysisIfAvailable()、getAnalysis()、getAnalysisID()这三个常用的函数，也都是在这个头文件中使用的。Pass之间的交互，是会经常用到这样的函数的。

1. ModulePass class   模块pass 类
      
       增加/删除 函数
       处理MetaData（元数据，在函数之外）

2. CallGraphSCCPass class

3. FunctionPass class  函数pass类
       
       处理函数内部的信息
       
       LLVM::Function类 getName() 成员函数 取得了函数的名字
       
       virtual bool runOnFunction(Function &F) 是 FunctionPass class 的虚函数
       一个FunctionPass的子类要想做一些实际的工作，就必须对这个虚函数进行实现。
       
       注册pass static RegisterPass<Hello> X("hello", "Hello World Pass");
       对hello PASS进行注册，只有PASS进行了注册之后，才可以进行使用。

[HELLO PASS](http://llvm.org/doxygen/Hello_8cpp_source.html)

4. LoopPass class     循环pass类

5. RegionPass class

6. BasicBlockPass class  基本快pass类

7. MachineFunctionPass class


>  LVM类           功能

    LLVMContext     上下文类，基本是最核心的保存上下文符号的类 环境相关部分，如Contexts
    Module          模块类，一般一个文件是一个模块，里面有函数列表和全局变量表
    Function        函数类，函数类，生成出来就是一个C函数
    Constant        常量类，各种常量的定义，都是从这里派生出来的
    Value           各值类型的基类，几乎所以的函数、常量、变量、表达式，都可以转换成Value型
    Type            类型类，表示各种内部类型或用户类型，每一个Value都有个getType方法来获取其类型。
    BasicBlock      基本块，一般是表示一个标签，注意这个块不是嵌套形式的结构，而是每个块结尾可以用指令跳转 到其他块，类似C语言中的标签的功能
    
Values中使用最多就是常量Constant，从上图可以看到Constant主要可以概括为以下几个部分:

    标量常量Scalar Constants，比如整形常量ConstantInt，浮点型常量ConstantFP
    组合常量Composite Constants，比如常量结构体ConstStruct，常量数组ConstantDataArray等
    常量表达式Constant Expressions，主要是ConstantExpr这个类，该类有一个及其重要的API接口getGetElementPtr用来从多元素常量中获取首地址，比如获取数组，字符串的首地址
    全局值Global Values，是全局变量和函数值的间接基类，换而言之就是全局值包括全局变量GlobalVariable和函数Function，全局值都有一个很重要的属性就是连接属性
    全局变量Global Variables
    全局别名Global Aliases
    函数值Function values
    

首先要介绍的是LLVM类型系统的使用，因为LLVM的每条语句都是带有类型的，LLVM语句可以转换成Value型指针，那么我们用如下的方法就可以获取到当前value的类型：

```c
Type* t = value->getType();
```

Type类型也很容易使用，例如获取其指针就可以：

```c
PointerType* ptr_type = t->getPointerTo(); // 获取指向 Type类型数据的指针
```

type类型中还有很多静态函数可供生成基本类型：

```c
/ 获取基本类型
static Type *   getVoidTy (LLVMContext &C) 
static Type *   getFloatTy (LLVMContext &C) 
static Type *   getDoubleTy (LLVMContext &C)
static Type *   getMetadataTy (LLVMContext &C)

// 获取不同长度整形类型
static IntegerType *    getInt8Ty (LLVMContext &C)
static IntegerType *    getInt16Ty (LLVMContext &C)
static IntegerType *    getInt32Ty (LLVMContext &C)
static IntegerType *    getInt64Ty (LLVMContext &C)

// 获取指向不同类型的指针类型
static PointerType *    getFloatPtrTy (LLVMContext &C, unsigned AS=0)
static PointerType *    getDoublePtrTy (LLVMContext &C, unsigned AS=0)
static PointerType *    getInt8PtrTy (LLVMContext &C, unsigned AS=0)
static PointerType *    getInt16PtrTy (LLVMContext &C, unsigned AS=0)
static PointerType *    getInt32PtrTy (LLVMContext &C, unsigned AS=0)
static PointerType *    getInt64PtrTy (LLVMContext &C, unsigned AS=0)



```

## Backend 后端
    1. TableGen            生成LLVM后端代码生成器（LLVMcodegenerator) 
       LLVM编译时，会调用TableGen的工具产生相应的类
    2. SelectionDAG        拓扑排序/线性化（‘指令调度’
    3. Scheduler           调度器
    4. MachineInstr
    5. Register allocation 寄存器分配
    6. Prologue and epilogue
    7. MCInst



## 运行时 JIT

Just-In-Time Compiler，是一种动态编译中间代码的方式，根据需要，在程序中编译并执行生成的机器码，能够大幅提升动态语言的执行速度。

JIT引擎的工作原理并没有那么复杂，本质上是将原来编译器要生成机器码的部分要直接写入到当前的内存中，然后通过函数指针的转换，找到对应的机器码并进行执行。

但实践中往往需要处理许多头疼的问题，例如内存的管理，符号的重定向，处理外部符号，相当于要处理编译器后端的诸多复杂的事情，真正要设计一款能用的JIT引擎还是非常困难的。

当然基本的功能是提供一款解释器的底层工具，将LLVM字节码解释执行，具体能够做的事，例如可以制作一款跨平台的C++插件系统，使用clang将C/C++代码一次编译到.bc字节码，然后在各个平台上解释运行。也可以制作一款云调试系统，联网远程向系统注册方法，获取C++客户端的debug信息等等。当然，还有很多其他的用法等着大家来开发。

制作LLVM字节码的解释器还是非常简单的，最棒的示例应该是LLVM源码中的工具：lli

一共700行左右的C++代码，调用LLVM工具集实现了LLVM字节码JIT引擎，如果想很好的学习llvm中的解释器和JIT，可以参考其在 [lli](https://github.com/llvm-mirror/llvm/blob/master/tools/lli/lli.cpp)

## 交叉编译   仿真平台 QEMU



## Clang静态分析
    1. （后端指令生成的）性能优化 & （基于前端AST的）静态分析，编译器技术的两大法宝！
    2. 竞争对手：HP Fortify and Synopsis Coverity
    3. exponential-time complexity，不支持inter-module analysis
    4. forward data flow analysis  数据流分析
        给变量符号关联一些属性，然后在后面用到的地方检查约束是否满足
        False positives：往往导致程序员忽略所有的警告信息






