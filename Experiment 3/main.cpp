/*
��ʵ�ּӷ��ķ���ģʽ
start->   DS.
D->B; D
D->��
B->int L    { L.type := int }|real L    { L.type := real }
L->id   { A.Type := L.type  enter(v.entry,L.type)}  A
A-> ��idA   { A1.Type := A.type  enter(v.entry,A.type)}
A->��

S�� V := E { gen( ":=", E.place,0,V.place) } H
H����S | ��
E->T    { R.i:=T.place}      R     {E.place:=R.s}
R->+T    { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
R-> ��    {Rs=R.i}
T->( E )  { F.place := E.place}
F->id     {F.place:=position (id)}
V->id     {V.place:=position(id)}
*/

#include "header.h"

int nowtype=0;

 int main()
{
    char filename[20];
	printf("������������ļ���:");
	gets(filename);
	if((fin=fopen(filename,"r"))==NULL)
	{
		printf("���ܴ��ļ�.\n");
		exit(0);
	}

    init();                  //��ʼ��
    getsym();                //����һ�����ʣ�������������sym�У����ִ�ֵ����id��
	start();                //��ʼ��start->DS.  ����

    if (sym==eeof)
	{
		printf("�﷨��ȷ\n\n������м���뱣�浽�ļ��������ļ���������س�");
		gets(filename);
		if(strlen(filename)<=0) return 0;
		if((fout=fopen(filename,"w"))==NULL)
		{
			printf("���ܴ��ļ�.\n");
			exit(0);
		}
		for (int cx1=0;cx1<cx;cx1++)
			fprintf(fout,"(%s,%s,%s,%s)\n",code[cx1].f,code[cx1].l,code[cx1].r,code[cx1].t);
		return 0;
	}
	else {printf("�﷨��1:  . �������о���"); exit(0);}
	fclose(fin);
	fclose(fout);
}



//��ʼ������
void init()
{
	int i;

	/* ���õ��ַ����� */
	for (i=0; i<=255; i++)
	{
		ssym[i] = nul;            //����ȷ�ĵ��ַ�����Ϊnul����Ԥ�ó�ֵnul
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

	/* ���ñ��������� */
	strcpy(&(word[0][0]), "real");
	strcpy(&(word[1][0]), "int");
	strcpy(&(word[2][0]), "bool");

	/* ���ñ����ַ��� */
	wsym[0] = realsym;
	wsym[1] = intsym;
	wsym[2] = boolsym;

	tv=100;           //��ʱ����ָ���ֵ����Tx��tv��ȡֵû�н��������𵽵�����ʱ������������ı���
	tx=0;           //��ָ���ֵ
	cx=0;     //ָ�������

}


/*
* �ʷ���������ȡһ������
*/
int getsym()
{
	int i,k;
	ch=fgetc(fin);

	if (ch==EOF) {sym=eeof; return 0;}         //�ļ�����

	while (ch==' ' || ch==10 || ch==13 || ch==9)  /* ���Կո񡢻��С��س���TAB */
		ch=fgetc(fin);

	if (ch>='a' && ch<='z')
	{           /* ���ֻ�������a..z��ͷ ��*/
		k = 0;
		do
		{
			if(k<al)                  /* ���ŵ���󳤶�Ϊal �������ͽ�β����*/
			{
				a[k] = ch;
				k++;
			}
			ch=fgetc(fin);
		} while (ch>='a' && ch<='z' || ch>='0' && ch<='9');

		a[k] = 0;
		strcpy(id, a);
		fseek(fin,-1,1);

		for (i=0;i<norw;i++)        /* ������ǰ�����Ƿ�Ϊ������ */
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
			//printf("%d",nowtype);       /* ����ʧ���������Ǳ�ʶ�� */
		}
	}
	else if(ch == ':') /* ��⸳ֵ���� */
	{
		ch=fgetc(fin);
		if (ch == '=')
		{
			sym = becomes;
		}
		else
		{
			sym = nul;  /* ����ʶ��ķ��� */
		}
	}
	else sym = ssym[ch];     /* �����Ų�������������ʱ��ȫ�����յ��ַ����Ŵ��� */
	return 0;
}


/*
* �ڷ��ű��м���һ��
*/

void enter(enum symbol type)
{
	int i;
	tx=tx+1;
	if (tx > txmax)
    {
		printf(" ���ű�Խ�� ");           /* ���ű�Խ�� */
		return;
	}
	for(i=0;i<tx;i++){
        if(strcmp(table[i].name,id)==0){
            printf("�����ظ�����");
            exit(0);
        }
	}
	strcpy(table[tx].name, id); /* ȫ�ֱ���id���Ѵ��е�ǰ���ֵ�����,TxΪ���뵱ǰ����֮ǰ��βָ�� */
	table[tx].type = sym;

}



/*
* �������ֵ�λ��.
* �ҵ��򷵻������ֱ��е�λ��,���򷵻�0.
*
* idt:    Ҫ���ҵ�����
* tx:     ��ǰ���ֱ�βָ�룬ȫ�ֱ���
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


/* �м����*/
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
        printf("bool���ܽ��д�������͹�ϵ����\n");
        exit(0);
	}
	if(op==becomes&&(table[arg1].type!=boole&&table[result].type==boole)){
        //printf("%d,%d",table[arg1].type,table[result].type);
        printf("bool���ܽ��и�ֵ\n");
        exit(0);
	}
	if(!(table[arg1].type==table[arg2].type||table[arg1].type==table[result].type)){
        //printf("%d,%d,%d,%d",table[arg1].type,table[arg2].type,arg1,arg2);
        printf("�������Ͳ�һ��\n");
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
	if(arg1>=100)                            //ģ��������ʱ����
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
	else if (op==plus)                          //+����
	{

		writecode("+",temp1,temp2,temp3);
		printf("(+,%s,%s,%s)\n",temp1,temp2,temp3);
	}
	else if (op==minus)                          //+����
	{

		writecode("-",temp1,temp2,temp3);
		printf("(-,%s,%s,%s)\n",temp1,temp2,temp3);
	}else if (op==multi)                          //+����
	{

		writecode("*",temp1,temp2,temp3);
		printf("(*,%s,%s,%s)\n",temp1,temp2,temp3);
	}
	else if (op==divide)                          //+����
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

//��������
void writecode(char op[al+1], char arg1[al+1], char arg2[al+1],char result[al+1] )
{
	if (cx >= cxmax)
	{
		printf("Program too long"); /* ������� */
		return ;
	}
	strcpy(code[cx].f, op);
	strcpy(code[cx].l,arg1);
	strcpy(code[cx].r,arg2);
	strcpy(code[cx].t,result);
	cx++;
	return ;
}


/*��������ʽ     P->DS.   */

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
		{printf("�﷨��2�� ȱ�ٳ������."); exit(0);}
	}
	else
	{printf("�﷨��3: ����ֻ����int,��real��ʼ���������ִ�Сд"); exit(0);}
}

/*�ݹ��½�����
D->  B; D
D->��

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
		{printf("�﷨��1"); exit(0);}
	}
	else if(sym==real||sym==inte||sym==boole || sym==period)  return;

	else {printf("�﷨��2"); exit(0);}
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
	{printf("�﷨��3"); exit(0);}
}


/*
L->   id   { A.Type := L.type  enter(v.entry,L.type)}   A       V.entryͨ��ȫ�ֱ���tx���Դ���
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
	{printf("�﷨��4"); exit(0);}
}


/*
A-> ��id  A   { A1.Type := A.type  enter(v.entry,A.type)}
A->��
*/

void A(enum symbol type)
{
	if (sym==comma)          //��ǰ����Ϊ��
	{
		getsym();
		if (sym==real||sym==inte||sym==boole)
		{
			enter(type);
			getsym();
			A(type);
		}
		else
		{printf("�﷨��5"); exit(0);}
	}
	else if (sym== semicolon)   return ;//��ǰ����Ϊ����A��follow��Ԫ�أ��൱�ڽ���A->��
	else
	{printf("�﷨��6"); exit(0);}
}



/*
S�� V := E { gen( ":=", E.place,0,V.place) } H
*/
void S()
{
	int vplace,Eplace,Eright,i;
	if (sym==real||sym==inte||sym==boole)
	{
		vplace=V();
		//getsym();
		if (sym==becomes)     //��ǰ����Ϊ:=
		{
			getsym();
			Eplace=E();
			gen(becomes,Eplace,-1,vplace);
			H(Eplace);
		}
		else
		{printf("�﷨��7"); exit(0);}
	}
	else {printf("�﷨��8"); exit(0);}
}

/*
H����S | ��
*/
void H(int last)
{   int Eright;

	if (sym==semicolon)          //��ǰ����Ϊindent����
	{
		getsym();
		S();
	}
	else if (sym==period)   return ;
	else
	{printf("�﷨��9"); exit(0);}
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
		{printf("�﷨��10"); exit(0);}
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
		{printf("�﷨��a"); exit(0);}
	return Rs;
}

/*
R->+T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
R->-T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
R-> ��    {R.s=R.i}
*/

int R(int Ri)
{
	int Rs,tplace;
	if (sym==plus)
	{
		getsym();
		tplace=T();
		tv=tv+1;            //������ʱ����
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
	else {printf("�﷨��11"); exit(0);}
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
		{printf("�﷨��12"); exit(0);}
	return Rs;
}

/*
P->*T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
P->/T     { R1.i:= newtemp; gen( "*", R.i, T.place , R1.i) }     R   {R.s:= R1.s; }
P-> ��    {R.s=R.i}
*/

int P(int Pi)
{
	int Rs,fplace;
	if (sym==multi)
	{
		getsym();
		fplace=F();
		tv=tv+1;            //������ʱ����
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
	else {printf("�﷨��13"); exit(0);}
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
		if (Tplace==0)  {printf("����û������"); exit(0);}
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
			printf("�﷨��ȱ)"); exit(0);
		}
	}
	else{printf("�﷨��,ȱ("); exit(0);}
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
		if (Vplace==0)  {printf("����û������"); exit(0);}
		getsym();
	}
	else{printf("�﷨��14"); exit(0);}
	return Vplace;
}
