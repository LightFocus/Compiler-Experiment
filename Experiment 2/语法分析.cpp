//
//  main.cpp
//  complier2
//
//  Created by Light Focus on 2019/4/1.
//  Copyright © 2019 Light Focus. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stack>

using namespace std;

enum symbol {
    nul,
    ident,
    plus,
    times,
    lparen,
    rparen,
    divide,
    sub,
    compare
};

//enum symbol sym;
 
 #define nul 0
 #define ident 1
 #define plus 2
 #define times 3
 #define lparen 4
 #define rparen 5
 #define divide 6
 #define sub 7
 #define compre 8

int sym,num=1,currentchar=1;    //当前的符号
char ch,past[50]="\0";

FILE* fin;

void getsym();
void V();
int E();

stack<char> A;  //堆栈存放接下来要分析的词，栈底为&表示分析结束
int result;

int main()
{
    char filename[20];
    
    printf("请输入分析的文件名:");
    gets(filename);
    do
    {
        if((fin=fopen(filename,"r"))==NULL)
        {
            printf("不能打开文件.\n");
            return 0;
        }
        getsym();            //读第一个单词，将单词类别放入sym中
        result=E();                //开始按E->TR分析
        if (result==1)   //分析正常结束
        {
            printf("语法正确\n");
        }
        else {
            printf("语法错误\n错误发生在：");
            puts(past);
            printf("附近\n");
        }
        fclose(fin);
        num=1;
        printf("继续分析则输入文件名，否则回车\n");
        gets(filename);
    } while (strlen(filename)>0);
    return 0;
}

/*
 * 词法分析，获取一个符号
 */
void getsym()
{
    ch=fgetc(fin);
    
    while (ch==' ' || ch==10 || ch==13 || ch==9)  /* 忽略空格、换行、回车和TAB */
        ch=fgetc(fin);
    
    if (ch==EOF)  sym=nul;
    
    else if (ch>='a' && ch<='z')
        sym = ident;
    else if(ch == '+')
        sym = plus;
    else if (ch == '*')
        sym = times;
    else if (ch == '/')
        sym = divide;
    else if (ch == '-')
        sym = sub;
    else if (ch == '(')
        sym = lparen;
    else if (ch == ')')
        sym = rparen;
    else if (ch =='>' ||ch =='<')
        sym = compare;
    else {
        printf("--词法错\n");
        exit(0);
    }
    return;
}


/*
 V->a|...|z
 */
void V()
{
    if (sym==ident||sym==lparen)          //首字母为变量或者(
        ;
    else
    {
        printf("语法错误: 缺变量\n");
        exit(0);}
}

void initstack()   //初始化堆栈，栈底为结束符&，并加入非终结符E以便开始自顶向下分析
{
    while(!A.empty())
    {
        A.pop();
    }
    A.push('&');
    A.push('E');
}

int test(char x,int y)   //x为栈顶字符，y为当前sym类型
{
    if(x == 'E')   //将E替换为TR
    {
        printf("%d ",num);
        printf("E-->TR\n");
        A.pop();
        A.push('R');
        A.push('T');
        num++;
        return 1;
    }
    else if(x == 'R'&&y == 2)  //将R替换为+TR
    {
        printf("%d ",num);
        printf("R-->+TR\n");
        A.pop();
        A.push('R');
        A.push('T');
        A.push('+');
        num++;
        return 1;
    }
    else if(x == 'R'&&y == 7)  //将R替换为-TR
    {
        printf("%d ",num);
        printf("R-->-TR\n");
        A.pop();
        A.push('R');
        A.push('T');
        A.push('-');
        num++;
        return 1;
    }
    else if(x == 'R'&&y != 2 && y != 7)  //将R替换为ε，即移除
    {
        printf("%d ",num);
        printf("R-->ε\n");
        A.pop();
        num++;
        return 1;
    }
    else if(x == 'T')  //将T替换为FP
    {
        printf("%d ",num);
        printf("T-->FP\n");
        A.pop();
        A.push('P');
        A.push('F');
        num++;
        return 1;
    }
    else if(x =='P' && y == 3)  //将P替换为*FP
    {
        printf("%d ",num);
        printf("P-->*FP\n");
        A.pop();
        A.push('P');
        A.push('F');
        A.push('*');
        num++;
        return 1;
    }
    else if(x =='P' && y == 6)  //将P替换为/FP
    {
        printf("%d ",num);
        printf("P-->/FP\n");
        A.pop();
        A.push('P');
        A.push('F');
        A.push('/');
        num++;
        return 1;
    }
    else if(x =='P' && y != 3 && y != 6)  //将P替换为ε，即移除
    {
        printf("%d ",num);
        printf("P-->ε\n");
        A.pop();
        num++;
        return 1;
    }
    else if(x == 'F' && y == 1)  //将F替换为V
    {
        printf("%d ",num);
        printf("F-->V\n");
        A.pop();
        A.push('V');
        num++;
        return 1;
    }
    else if(x == 'F' && y == 4)  //将F替换为(E)
    {
        printf("%d ",num);
        printf("F-->(E)\n");
        A.pop();
        A.push(')');
        A.push('E');
        A.push('(');
        num++;
        return 1;
    }
    else
    {
        if(x == '+' && y == 2)  //若栈顶与当前字符相同，则将其移出栈，并读取下一个字符，下同
        {
            A.pop();
            return 2;
        }else if(x == '-' && y == 7){
            A.pop();
            return 2;
        }else if(x == 'V' && y == 1){
            A.pop();
            return 2;
        }else if(x == '*' && y == 3){
            A.pop();
            return 2;
        }else if(x == '/' && y == 6){
            A.pop();
            return 2;
        }else if(x == '(' && y == 4){
            A.pop();
            return 2;
        }else if(x == ')' && y == 5){
            A.pop();
            return 2;
        }
        else
        {
            return 0;
        }
    }
}

int E()
{
    V();
    past[0]=ch;
    initstack();
    while(1)
    {
        if(sym==8){  //若读取到>或<，则说明前面所有部分为一个E，则读取下一个字符并把它当成一个新的E开始分析
            if(A.top() != '&'){  //栈不空，说明大于小于号前面的E不完整，报错
                return 0;
            }
            getsym();
            if(sym==8){
                printf("同时出现两个大于/小于号");
                return 0;
            }
            V();
            initstack();
        }
        if(A.top() == '&' && sym == 0) //分析结束
        {
            return 1;
        }
        else
        {
            int key = test(A.top(),sym);
            if(key == 0)
            {
                return 0;
            }else if(key == 2){  //读下一个字符进行分析
                getsym();
                past[currentchar]=ch;
                currentchar++;
            }
        }
    }
}
