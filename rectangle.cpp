#include<iostream>
using namespace std;

class rectangle
{
    friend bool isSquare(const rectangle& rect);
public:
    void area()   //  面积函数
    {
        cout << "矩形的面积是： "<< (*width)*(*height)<<endl;
    }
    rectangle(double input_width,double input_height)    // 有参构造函数
    {
        width = new double(input_width);
        height = new double(input_height);
        cout<<"构造函数"<<endl;
    }
    ~rectangle(){      //  析构函数
        if (width) delete width;
        if (height) delete height;
        cout<<"析构函数"<<endl;
    }
    rectangle(const rectangle &r){    //拷贝构造函数
        width = new double(*r.width);
        height = new double(*r.height);
    }
    void resize(double scale)    // 等比例缩小成员函数
    {
        *width = (double)*width/scale;
        *height = (double)*height/scale;
    }
    // 赋值运算符重载
    rectangle& operator=(rectangle& rec)
    {   
        // 判断是否有属性在堆区，如果有先释放干净，然后再深拷贝
        if(width != NULL && height != NULL)
        {
            delete width;
            delete height;
            width = NULL;
            height = NULL;
        };
        // 深拷贝
        width = new double(*rec.width);
        height = new double(*rec.height); 
        return *this;
    }
    

private:   
    double *width;
    double *height;
};
bool isSquare(const rectangle& rect)
{
    if(*rect.width == *rect.height){
        cout<<"该矩形是正方形"<<endl;
        return true;
    }
    else{
        cout<<"该矩形不是正方形"<<endl;
        return false;
    }
}

void test01()
{
    rectangle r1(10.0,10.0);   //  示例
    rectangle r3(13.0,15.0);
    isSquare(r1);
    isSquare(r3);
    rectangle r2(r1);
    cout<<"这是r2的面积： ";
    r2.area();
    r3.area();   
}

int main()
{
    test01();     // 记得带括号调用函数
    return 0;
}
