#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

int main(){
    int n,x,y;
    scanf("%d",&n);
    char temp[100],str[100];  // 定义两个字符数组，其中temp用来后面的sprintf()
    char k;  // 用它来进行运算方法的判断
    for(int i=0;i<n;i++){
        scanf("%s",str);
        if(str[0]>='a'&&str[0]<='z'){  // 当第一个字符为字母时,则将它赋值给K，若是数字，则k不会改变，还有isalpha()是C++的用法，C中只有isalnum()
            k=str[0];   // 接上行，isalnum()用于检查一个字符是否是字母或数字。对此题不太适用
            scanf("%d %d",&x,&y);}  // 读取接着的两个整数
            else{
                x=atoi(str);  // 如果不是字母，那就利用atoi将str的首元素转为int类型
                scanf("%d",&y); // 另一个直接用Int类型输入
            }
            if(k=='a'){
                sprintf(temp,"%d+%d=%d",x,y,x+y);   // 向temp字符串写入内容
            }
            if(k=='b'){
                sprintf(temp,"%d-%d=%d",x,y,x-y);
            }
            if(k=='c'){
                sprintf(temp,"%d*%d=%d",x,y,x*y);
            }
            printf("%s\n%d\n",temp,strlen(temp));  // 注意strlen 是每次只读一个位置的元素的，所以如果是25的话，那就长度为2，不是作为单个字符的
        }
    return 0;
}
