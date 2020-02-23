
// 核心要理解引用计数，什么时候销毁底层指针，还有赋值，拷贝构造时候的引用计数的变化，析构的时候要判断底层指针的引用计数为0了才能真正释放底层指针的内存

template <typename T>

class SmartPtr
{

private:
    T *ptr; //底层真实的指针

    int *use_count; //保存当前对象被多少指针引用计数

public:
    SmartPtr(T *p); //SmartPtr<int>p(new int(2));

    SmartPtr(const SmartPtr<T> &orig); //SmartPtr<int>q(p);

    SmartPtr<T> &operator=(const SmartPtr<T> &rhs); //q=p

    ~SmartPtr();

    T operator*(); //为了能把智能指针当成普通指针操作定义解引用操作

    T *operator->(); //定义取成员操作

    T *operator+(int i); //定义指针加一个常数

    int operator-(SmartPtr<T> &t1, SmartPtr<T> &t2); //定义两个指针相减

    void getcount() { return *use_count }
};

template <typename T>
int SmartPtr<T>::operator-(SmartPtr<T> &t1, SmartPtr<T> &t2) { return t1.ptr - t2.ptr; }

template <typename T>
SmartPtr<T>::SmartPtr(T *p)
{
    ptr = p;
    try
    {
        use_count = new int(1);
    }
    catch (...)
    {
        delete ptr; //申请失败释放真实指针和引用计数的内存

        ptr = nullptr;
        delete use_count;
        use_count = nullptr;
    }
}
template <typename T>
SmartPtr<T>::SmartPtr(const SmartPtr<T> &orig) //复制构造函数
{

    use_count = orig.use_count; //引用计数保存在一块内存，所有的SmarPtr对象的引用计数都指向这里

    this->ptr = orig.ptr;

    ++(*use_count); //当前对象的引用计数加1
}

template <typename T>
SmartPtr<T> &SmartPtr<T>::operator=(const SmartPtr<T> &rhs)
{
    // 重载=运算符，例如SmartPtr<int>p,q; p=q;
    // 这个语句中，首先给q指向的对象的引用计数加1，因为p重新指向了q所指的对象，所以p需要先给原来的对象的引用计数减1，
    // 如果减一后为0，先释放掉p原来指向的内存，然后讲q指向的对象的引用计数加1后赋值给p

    ++*(rhs.use_count);
    // 除去多了这一部分的判断，因为如果使用了赋值重载， 要先解引用， 而解引用的过程中可能会使得当前的对象减少
    if ((--*(use_count)) == 0)
    {
        delete ptr;
        ptr = nullptr;
        delete use_count;
        use_count = nullptr;
    }
    ptr = rhs.ptr;
    *use_count = *(rhs.use_count);
    return *this;
}
template <typename T>
SmartPtr<T>::~SmartPtr()
{
    getcount();
    // SmartPtr的对象会在其生命周期结束的时候调用其析构函数，在析构函数中检测当前对象的引用计数是不是只有正在结束生命周期的这个SmartPtr引用，
    // 如果是，就释放掉，
    // 如果不是，就还有其他的SmartPtr引用当前对象，就等待其他的SmartPtr对象在其生命周期结束的时候调用析构函数释放掉

    if (--(*use_count) == 0) 
    {
        getcount();
        delete ptr;
        ptr = nullptr;
        delete use_count;
        use_count = nullptr;
    }
}
template <typename T>
T SmartPtr<T>::operator*()
{
    return *ptr;
}
template <typename T>
T *SmartPtr<T>::operator->()
{
    return ptr;
}

template <typename T>
T *SmartPtr<T>::operator+(int i)
{
    T *temp = ptr + i;
    return temp;
}
