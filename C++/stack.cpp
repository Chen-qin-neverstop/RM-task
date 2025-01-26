#include<iostream>
using namespace std;

template<class T>
class stack
{
public:
    //  构造函数  用指针维护数组,同时还要对容量和栈顶索引初始化
    stack()
    {   
        this->array = new T[m_capacity];
        topindex = -1;
        m_capacity = 3;
    };
    //  析构函数
    ~stack()
    {
        delete[] array;    
    }

    // 入栈函数
    void push(T value)   // 如果索引到了m_capacity-1,就需要重新对数组进行扩容
    {   
        if(topindex == m_capacity -1)
        {
            resize();
        }
        cout<<"输入栈： "<<value<<endl;
        array[++topindex] = value;    
    }
    // 出栈函数
    void pop()    // 将栈顶的元素弹出，其实可以逻辑上抹杀它，让索引向前进一位就好,但是要注意如果已经空的话，那还前进个鸡毛
    {   
        if(isEmpty())   // 如果索引已经为-1，那直接报错，然后返回
        {
            cout<<"栈已经清空"<<endl;
            return ;
        }
        topindex-=1;
    }
    // 返回栈顶元素
    T top()   
    {
        if(isEmpty())   // 如果索引已经为-1，那直接报错，然后返回
        {
            cout<<"栈已经清空"<<endl;
        }
        return array[topindex];
    }
    // 查空函数
    bool isEmpty()
    {
        if(topindex == -1)
        {
            return true;
        }
        else{return false;}
    }
    // 拷贝构造函数
    stack(const stack<T>& s)
    {
        cout<<"拷贝函数开始执行"<<endl;
        this->array = new T[s.m_capacity];
        this->topindex = s.topindex;
        this->m_capacity = s.m_capacity;
        // 对栈内的元素进行逐一赋值
        for(int i = 0;i!=topindex+1;i++)   // 最好还是用topindex，直接用m_capacity容易循环次数过多
        {
            this->array[i] = s.array[i];
        }
    }
    // 赋值运算符重载  其实现其实和拷贝差不多
    stack& operator=(const stack<T> &s)
    {
        this->array = new T[s.m_capacity];
        this->m_capacity = s.m_capacity;
        this->topindex = s.topindex;
        // 对栈内的元素进行逐一赋值
        for(int i = 0;i!=topindex+1;i++)  
        {
            this->array[i] = s.array[i];
        }
        return  *this;
    }
    int size()
    {
        return topindex+1;
    }
private:
    T *array;   // 创建动态数组的指针来存储栈元素
    //那么显然要在堆区创建一个新的存储空间，并用这个指针维护
    int m_capacity;  // 动态数组容量
    //但是由于数组大小未知，所以m_capacity要改变，那就要再添加一个重新改变数组大小的函数resize()，而且这个resize还是不能对外开放的
    int topindex;    //  栈顶元素索引
    void resize()
    {   
        //cout<<"扩大栈容量"<<endl;
        int origin = m_capacity;  // 原来的元素的个数
        m_capacity+=10;
        T* new_array = new T[m_capacity];   // 这里用新的指针维护，arrar指针此时还保存着原来的数据，后边赋值用，复制完再用新的指针刷新arrar
        // 对原来的元素，进行赋值操作 ,
        for(int i = 0;i < origin;i++)
        {
            new_array[i] = this->array[i];
        }
        // 刷新arrar
        delete[] array;
        array = new_array;
    }
};
void test01()
{
    stack<int>s;
    // 入栈
    s.push(10);
    s.push(20);
    s.push(30);
    s.push(40);
    s.push(50);
    // // 查看+出栈
    // while (!s.isEmpty())
    // {
    //     cout<<"栈顶元素为："<<s.top()<<endl;
    //     s.pop();
    //     cout<<"弹出后栈的大小为： "<<s.size()<<endl;
    // }
    stack<int>s1;
    s1=s;
    // while (!s1.isEmpty())
    // {
    //     cout<<"s1栈顶元素为："<<s1.top()<<endl;
    //     s1.pop();
    //     cout<<"弹出后栈s1的大小为： "<<s1.size()<<endl;
    // }
    stack<int>s2(s1);    //记得要在拷贝对象初始化后再调用
    while (!s2.isEmpty())
    {
        cout<<"拷贝构造函数执行"<<endl;
        cout<<"s2栈顶元素为："<<s2.top()<<endl;
        s2.pop();
        cout<<"弹出后栈s2的大小为： "<<s2.size()<<endl;
    }
}
int main()
{
    test01();
    return 0;
}
