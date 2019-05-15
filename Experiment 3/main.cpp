/*
仅实现加法的翻译模式
start->   DS.
D->B; D
D->ε
B->int L    { L.type := int }|real L    { L.type := real }
L->id   { A.Type := L.type  enter(v.entry,L.type)}  A
A-> ，idA   { A1.Type := A.type  enter(v.entry,A.type)}
A->ε

S→ V := E { gen( ":=", E.place,0,V.place) } H
H→；S | ε
E->T    { R.i:=T.place}      R     {E.place:=R.s}
R->+T    { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
R-> ε    {Rs=R.i}
T->( E )  { F.place := E.place}
F->id     {F.place:=position (id)}
V->id     {V.place:=position(id)}
*/

#include "header.h"

int nowtype=0;

 int main()
{
    char filename[20];
	printf("请输入分析的文件名:");
	gets(filename);
	if((fin=fopen(filename,"r"))==NULL)
	{
		printf("不能打开文件.\n");
		exit(0);
	}

    init();                  //初始化
    getsym();                //读第一个单词，将单词类别放入sym中，单字词值放入id中
	start();                //开始按start->DS.  分析

    if (sym==eeof)
	{
		printf("语法正确\n\n如果将中间代码保存到文件请输入文件名，否则回车");
		gets(filename);
		if(strlen(filename)<=0) return 0;
		if((fout=fopen(filename,"w"))==NULL)
		{
			printf("不能打开文件.\n");
			exit(0);
		}
		for (int cx1=0;cx1<cx;cx1++)
			fprintf(fout,"(%s,%s,%s,%s)\n",code[cx1].f,code[cx1].l,code[cx1].r,code[cx1].t);
		return 0;
	}
	else {printf("语法错1:  . 后不能再有句子"); exit(0);}
	fclose(fin);
	fclose(fout);
}



//初始化函数
void init()
{
	int i;

	/* 设置单字符符号 */
	for (i=0; i<=255; i++)
	{
		ssym[i] = nul;            //不正确的单字符符号为nul，先预置初值nul
	}
	ssym['+'] = plus;
	ssym['-'] = minus;
	ssym['*'] = multi;
	ssym['/'] = divide;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['.'] = period;
	ssym[','] = comma;
	ssym[';'] = semicolon;
	ssym['>'] = greater;
	ssym['<'] = less;

	/* 设置保留字名字 */
	strcpy(&(word[0][0]), "real");
	strcpy(&(word[1][0]), "int");
	strcpy(&(word[2][0]), "bool");

	/* 设置保留字符号 */
	wsym[0] = realsym;
	wsym[1] = intsym;
	wsym[2] = boolsym;

	tv=100;           //临时变量指针初值，让Tx和tv的取值没有交集，区别到底是临时变变量和声明的变量
	tx=0;           //表指针初值
	cx=0;     //指令计数器

}


/*
* 词法分析，获取一个符号
*/
int getsym()
{
	int i,k;
	ch=fgetc(fin);

	if (ch==EOF) {sym=eeof; return 0;}         //文件结束

	while (ch==' ' || ch==10 || ch==13 || ch==9)  /* 忽略空格、换行、回车和TAB */
		ch=fgetc(fin);

	if (ch>='a' && ch<='z')
	{           /* 名字或保留字以a..z开头 ，*/
		k = 0;
		do
		{
			if(k<al)                  /* 符号的最大长度为al ，超长就截尾处理*/
			{
				a[k] = ch;
				k++;
			}
			ch=fgetc(fin);
		} while (ch>='a' && ch<='z' || ch>='0' && ch<='9');

		a[k] = 0;
		strcpy(id, a);
		fseek(fin,-1,1);

		for (i=0;i<norw;i++)        /* 搜索当前符号是否为保留字 */
			 if (strcmp(id,word[i]) == 0)
					 break;
		if (i <norw)
		{
			sym = wsym[i];
			if(i==0){
                nowtype=16;
			}else if(i==1){
                nowtype=17;
            }else if(i==2){
                nowtype=18;
            }
		}
		else
		{
			sym = (symbol)nowtype;
			//printf("%d",nowtype);       /* 搜索失败则，类型是标识符 */
		}
	}
	else if(ch == ':') /* 检测赋值符号 */
	{
		ch=fgetc(fin);
		if (ch == '=')
		{
			sym = becomes;
		}
		else
		{
			sym = nul;  /* 不能识别的符号 */
		}
	}
	else sym = ssym[ch];     /* 当符号不满足上述条件时，全部按照单字符符号处理 */
	return 0;
}


/*
* 在符号表中加入一项
*/

void enter(enum symbol type)
{
	int i;
	tx=tx+1;
	if (tx > txmax)
    {
		printf(" 符号表越界 ");           /* 符号表越界 */
		return;
	}
	for(i=0;i<tx;i++){
        if(strcmp(table[i].name,id)==0){
            printf("变量重复申明");
            exit(0);
        }
	}
	strcpy(table[tx].name, id); /* 全局变量id中已存有当前名字的名字,Tx为插入当前符号之前表尾指针 */
	table[tx].type = sym;

}



/*
* 查找名字的位置.
* 找到则返回在名字表中的位置,否则返回0.
*
* idt:    要查找的名字
* tx:     当前名字表尾指针，全局变量
*/
int position(char* idt)
{
	int i;
	strcpy(table[0].name, idt);
	i = tx;
	while (strcmp(table[i].name, idt) != 0)
	{
		i--;
	}
	return i;
}


/* 中间代码*/
int gen(enum symbol op, int arg1, int arg2,int result )
{
	int i;
	/*for(i=0;i<10;i++){
        printf("%d:",i);
        puts(table[i].name);
        printf("%d\n",table[i].type);
	}
	for(i=99;i<107;i++){
        printf("%d:",i);
        puts(table[i].name);
        printf("%d\n",table[i].type);
	}*/
	char temp1[al+1],temp2[al+1],temp3[al+1];
	if((op==plus||op==minus||op==multi||op==divide)&&(table[arg1].type==boole||table[arg2].type==boole)&&(table[arg1].type!=boole&&table[arg2].type!=boole)){
        printf("bool不能进行代数运算和关系运算\n");
        exit(0);
	}
	if(op==becomes&&(table[arg1].type!=boole&&table[result].type==boole)){
        //printf("%d,%d",table[arg1].type,table[result].type);
        printf("bool不能进行赋值\n");
        exit(0);
	}
	if(!(table[arg1].type==table[arg2].type||table[arg1].type==table[result].type)){
        //printf("%d,%d,%d,%d",table[arg1].type,table[arg2].type,arg1,arg2);
        printf("变量类型不一致\n");
        /*for(i=0;i<5;i++){
	    printf("%d:",i);
        puts(table[i].name);
        printf("%d\n",table[i].type);
        }
        for(i=99;i<105;i++){
	    printf("%d:",i);
        puts(table[i].name);
        printf("%d\n",table[i].type);
        }*/
        exit(0);
	}
	if(arg1>=100)                            //模拟申请临时变量
	{
		wsprintf(temp1,"T%d",arg1);
	}
	else
	{
		strcpy(temp1, table[arg1].name);
	}

	if(arg2>=100)
	{
		wsprintf(temp2,"T%d",arg2);
	}
	else
	{
		strcpy(temp2, table[arg2].name);
	}

	if(result>=100)
	{
		wsprintf(temp3,"T%d",result);
		strcpy(table[result].name,temp3);
		table[result].type=table[arg1].type;
	}
	else
	{
		strcpy(temp3,table[result].name);
	}

	if (op==becomes)
	{

		printf("(:=,%s,%s,%s)\n",temp1,temp2,temp3);
		writecode(":=",temp1,temp2,temp3);
	}
	else if (op==plus)                          //+运算
	{

		writecode("+",temp1,temp2,temp3);
		printf("(+,%s,%s,%s)\n",temp1,temp2,temp3);
	}
	else if (op==minus)                          //+运算
	{

		writecode("-",temp1,temp2,temp3);
		printf("(-,%s,%s,%s)\n",temp1,temp2,temp3);
	}else if (op==multi)                          //+运算
	{

		writecode("*",temp1,temp2,temp3);
		printf("(*,%s,%s,%s)\n",temp1,temp2,temp3);
	}
	else if (op==divide)                          //+运算
	{

		writecode("/",temp1,temp2,temp3);
		printf("(/,%s,%s,%s)\n",temp1,temp2,temp3);
	}else if (op==greater)
	{
        writecode(">",temp1,temp2,temp3);
		printf("(>,%s,%s,%s)\n",temp1,temp2,temp3);
		table[result].type=boole;
	}else if(op==less)
	{
	    writecode("<",temp1,temp2,temp3);
		printf("(<,%s,%s,%s)\n",temp1,temp2,temp3);
		table[result].type=boole;
	}

	return 0;
}

//处理代码段
void writecode(char op[al+1], char arg1[al+1], char arg2[al+1],char result[al+1] )
{
	if (cx >= cxmax)
	{
		printf("Program too long"); /* 程序过长 */
		return ;
	}
	strcpy(code[cx].f, op);
	strcpy(code[cx].l,arg1);
	strcpy(code[cx].r,arg2);
	strcpy(code[cx].t,result);
	cx++;
	return ;
}


/*分析产生式     P->DS.   */

void  start()
{
	if (sym==intsym ||sym==realsym||sym==boolsym)
	{
		D();
		S();
		if (sym==period)
		{
			getsym();
			return;
		}
		else
		{printf("语法错2： 缺少程序结束."); exit(0);}
	}
	else
	{printf("语法错3: 程序只能用int,和real开始，而且区分大小写"); exit(0);}
}

/*递归下降分析
D->  B; D
D->ε

*/
void  D()
{
	if (sym==intsym ||sym==realsym||sym==boolsym)
	{
		B();
		if (ch=';')
		{
			getsym();
			D();
		}
		else
		{printf("语法错1"); exit(0);}
	}
	else if(sym==real||sym==inte||sym==boole || sym==period)  return;

	else {printf("语法错2"); exit(0);}
}

/*
B->   int L    { L.type := int }|real L    { L.type := real }
*/
void  B()
{
	if (sym==intsym )
	{
		getsym();
		L(intsym);
	}
	else if (sym==realsym)
	{
		getsym();
		L(realsym);
	}else if(sym==boolsym){
        getsym();
        L(boolsym);
	}
	else
	{printf("语法错3"); exit(0);}
}


/*
L->   id   { A.Type := L.type  enter(v.entry,L.type)}   A       V.entry通过全局变量tx隐性传递
*/
void L(enum symbol type)
{
	if (sym==real||sym==inte||sym==boole)
	{
		enter(type);
		getsym();
		A(type);
	}
	else
	{printf("语法错4"); exit(0);}
}


/*
A-> ，id  A   { A1.Type := A.type  enter(v.entry,A.type)}
A->ε
*/

void A(enum symbol type)
{
	if (sym==comma)          //当前单词为，
	{
		getsym();
		if (sym==real||sym==inte||sym==boole)
		{
			enter(type);
			getsym();
			A(type);
		}
		else
		{printf("语法错5"); exit(0);}
	}
	else if (sym== semicolon)   return ;//当前单词为；即A的follow集元素，相当于进行A->ε
	else
	{printf("语法错6"); exit(0);}
}



/*
S→ V := E { gen( ":=", E.place,0,V.place) } H
*/
void S()
{
	int vplace,Eplace,Eright,i;
	if (sym==real||sym==inte||sym==boole)
	{
		vplace=V();
		//getsym();
		if (sym==becomes)     //当前单词为:=
		{
			getsym();
			Eplace=E();
			gen(becomes,Eplace,-1,vplace);
			H(Eplace);
		}
		else
		{printf("语法错7"); exit(0);}
	}
	else {printf("语法错8"); exit(0);}
}

/*
H→；S | ε
*/
void H(int last)
{   int Eright;

	if (sym==semicolon)          //当前单词为indent类型
	{
		getsym();
		S();
	}
	else if (sym==period)   return ;
	else
	{printf("语法错9"); exit(0);}
}
/*
E->T      { R.i:=T.place}      R     {E.place:=R.s}
*/

int E()
{
	int ri,tplace,Rs,result;
	if (sym==real||sym==inte||sym==boole || sym== lparen)
	{
		tplace=T();
		ri=tplace;
		Rs=R(ri);
		result=E1(Rs);
		if(result>100){
            Rs=result;
		}
	}
	else
		{printf("语法错10"); exit(0);}
	return Rs;
}
/*
E1->T      { R.i:=T.place}      R     {E.place:=R.s}
*/

int E1(int pre)
{
	int ri,Eplace,Rs;
	//printf("%d",sym);
	if (sym==greater)
	{
		getsym();
		Eplace=E();
		tv=tv+1;
		gen(greater,pre,Eplace,tv);
		Rs=tv;
	}else if(sym==less)
	{
        getsym();
		Eplace=E();
		tv=tv+1;
		gen(less,pre,Eplace,tv);
		Rs=tv;
	}else if(sym==semicolon||sym==period||sym==rparen)
	{
	    return -1;
	}
	else
		{printf("语法错a"); exit(0);}
	return Rs;
}

/*
R->+T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
R->-T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
R-> ε    {R.s=R.i}
*/

int R(int Ri)
{
	int Rs,tplace;
	if (sym==plus)
	{
		getsym();
		tplace=T();
		tv=tv+1;            //生成临时变量
		gen(plus,Ri,tplace,tv);
		Rs=R(tv);
	}else if(sym==minus)
	{
	    getsym();
	    tplace=T();
	    tv=tv+1;
	    gen(minus,Ri,tplace,tv);
	    Rs=R(tv);
	}
	else if (sym== semicolon || sym==rparen|| sym==period||sym==multi||sym==divide||sym==greater||sym==less)
	{
		Rs=Ri;
	}
	else {printf("语法错11"); exit(0);}
	return Rs;
}

/*
T->F      { R.i:=F.place}      P     {T.place:=P.s}
*/

int T()
{
	int ri,fplace,Rs;
	if (sym==real||sym==inte||sym==boole || sym== lparen)
	{
		fplace=F();
		ri=fplace;
		Rs=P(ri);
	}
	else
		{printf("语法错12"); exit(0);}
	return Rs;
}

/*
P->*T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
P->/T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
P-> ε    {R.s=R.i}
*/

int P(int Pi)
{
	int Rs,fplace;
	if (sym==multi)
	{
		getsym();
		fplace=F();
		tv=tv+1;            //生成临时变量
		gen(multi,Pi,fplace,tv);
		Rs=P(tv);
	}else if(sym==divide)
	{
	    getsym();
	    fplace=F();
	    tv=tv+1;
	    gen(divide,Pi,fplace,tv);
	    Rs=P(tv);
	}
	else if (sym== semicolon || sym==rparen|| sym==period||sym==plus||sym==minus||sym==greater||sym==less)
	{
		Rs=Pi;
	}
	else {printf("语法错13"); exit(0);}
	return Rs;
}

/*

F->( E )  { T.place := E.place}
F->id     {F.place:=position (id)}
*/
int F()
{
	int Tplace;
	if (sym==real||sym==inte||sym==boole)
	{
		Tplace=position (id);
		if (Tplace==0)  {printf("变量没有声明"); exit(0);}
		getsym();
	}
	else if (sym== lparen)
	{
		getsym();
		Tplace=E();
		if (sym== rparen)
			getsym();
		else
		{
			printf("语法错，缺)"); exit(0);
		}
	}
	else{printf("语法错,缺("); exit(0);}
	return Tplace;
}



/*

V->id     {V.place:=position(id)}
*/
int V()
{
	int Vplace;
	if (sym==real||sym==inte||sym==boole)
	{
		Vplace=position (id);
		if (Vplace==0)  {printf("变量没有声明"); exit(0);}
		getsym();
	}
	else{printf("语法错14"); exit(0);}
	return Vplace;
}
