## 为什么 CircuitBreaker 不需要 public CircuitBreaker() { setNativePtr(nativeCreate()); }
对比项目中两种构造模式：

### 模式1：公开构造 — 自己创建原生对象（如 AlertEngine）
```
public AlertEngine() {
    setNativePtr(nativeCreate());  // Java端主动创建
    C++对象
}

AlertEngine(long nativePtr) {      // 包级私有，从C
++回调接收指针
    setNativePtr(nativePtr);
}
```
### 模式2：仅包级私有构造 — 由C++端创建（如 CircuitBreaker）
```
CircuitBreaker(long nativePtr) {   // 只有这一个构造
器
    setNativePtr(nativePtr);
}
```
CircuitBreaker 不需要公开的无参构造器，原因有三：

### 1. 创建语义不同
CircuitBreaker 的 nativeCreate 需要参数：

```
private static native long nativeCreate(int 
failureThreshold, int recoveryTimeoutMs);
```
它不是一个"无参就能创建"的对象——必须指定 失败阈值 和 恢复超时时间 。如果提供无参构造器，要么传默认值（隐藏了重要配置），要么无法创建有意义的对象。所以设计上 不允许外部直接 new CircuitBreaker() 。

### 2. 对象所有权来源不同
模式 谁创建C++对象 谁拥有所有权 典型场景 公开构造 Java端主动调用 nativeCreate() Java端 AlertEngine、TileKey 等独立对象 包级私有构造 C++端创建后把指针传回Java C++端创建，Java端负责dispose CircuitBreaker、Alert 等从属对象

CircuitBreaker 通常是 ErrorRecoveryManager 的内部组件 ，由C++端的 ErrorRecoveryManager 创建后，通过JNI回调把指针传给Java端：

```
C++ 创建 CircuitBreaker → 返回 long ptr → Java端 
new CircuitBreaker(ptr)
```
### 3. 这是JNI封装的常见设计模式
在本项目中可以归纳为：

```
如果 Java 端需要主动创建 → 提供 public 构造器 + 
setNativePtr(nativeCreate(...))
如果 C++ 端创建后传回    → 只提供 包级私有 NativeObject
(long) 构造器
```
类似的例子：

- Alert 类：由 AlertEngine.getAlert() 返回，只有 Alert(long nativePtr) 构造器
- Feature 类：由 Layer 查询返回，只有 Feature(long nativePtr) 构造器
- AisTarget 类：由 AisManager 返回，只有 AisTarget(long nativePtr) 构造器
### 总结
不是"派生类不需要"，而是"这个类的语义决定了它不应该由Java端直接创建"。 如果将来需要一个从Java端创建的CircuitBreaker，应该提供带参数的公开构造器：