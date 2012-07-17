#include<stdio.h>#include<string.h>#include<time.h>#include<stdlib.h>#define size 10char stack[size];int tos=0,ele;void push();char pop();void show();int isempty();int isfull();char infix[30],output[30];int prec(char);int convert(char c){    if ((c>='0')&&(c<='9')){        return c-'0';    }    else return -1;}int value(int* a,int *b,char c ){    if (c=='+') return (*a)+(*b);    if (c=='-') return (*a)-(*b);    if (c=='*') return (*a)*(*b);    if (c=='/') return (*a)/(*b);    }int calculate(char infix[30]){    int length;    int i;    int output[30];    int number[30];    char oper[30];    int topnumber=0;    int topoper=0;    int j,k;    length=strlen(infix);    for(i=0;i<length;i++)    {        if (i==0){//push the first element to stack            if ((infix[i]>='0')&&(infix[i]<='9')){                number[topnumber]=convert(infix[i]);                topnumber++;             }            else {                oper[topoper]=infix[i];                topoper++;            }        }        else {            //push number to number stack            if ((infix[i]>='0')&&(infix[i]<='9'))            {                if ((infix[i-1]>='0')&&(infix[i-1]<='9'))                {                    number[topnumber-1]=number[topnumber-1]*10+convert(infix[i]);                }                else {                    number[topnumber]=convert(infix[i]);                    topnumber++;                }            }            else {                //push operation to operation stack                //push ( to operation stack                if (infix[i]=='('){                    oper[topoper]=infix[i];                    topoper++;                }                else if(infix[i]==')')                {                    while (oper[topoper-1]!='(')                    {                        number[topnumber-2]=value(&number[topnumber-2],&number[topnumber-1],oper[topoper-1]);                        topnumber--;                        topoper--;                    }                    topoper--;                }                else if(prec(infix[i])>prec(oper[topoper-1])){                    oper[topoper]=infix[i];                    topoper++;                }                                else {                    while ((topoper>=1)&&(prec(infix[i])<=prec(oper[topoper-1]))){                        number[topnumber-2]=value(&number[topnumber-2],&number[topnumber-1],oper[topoper-1]);                         topnumber--;                        topoper--;                    }                    oper[topoper]=infix[i];                    topoper++;                }            }        }            }        while (topnumber>1){        number[topnumber-2]=value(&number[topnumber-2],&number[topnumber-1],oper[topoper-1]);         topnumber--;        topoper--;            }    //printf("result:%d\n",number[0]);    return number[0];    }int randomExpression(char filename[100] ){    FILE * file;    char question[30];    char sample[30];    int a,b,c;int i,j,k;    file=fopen(filename,"w");    srand(time(NULL));    //create level 1 question    for (k=0;k<90;k++){        strcpy(question,"");                for (i=0;i<3;i++){            a=rand();            b=a%9+1;            if (b<0) b=-b;            sprintf(sample,"%d",b);            strcat(question,sample);            c=a%10;            if (c<2) strcat(question,"*");            if ((c>=2)&&(c<6)) strcat(question,"+");            if ((c>=6)&&(c<=9)) strcat(question,"-");                }        a=rand();        b=a%9+1;        sprintf(sample,"%d",b);        strcat(question,sample);        fprintf(file,"%s %d\n",question,calculate(question));                    }        //create level 2 question    for (k=0;k<90;k++){        strcpy(question,"");                for (i=0;i<5;i++){            a=rand();            b=a%9+1;            if (b<0) b=-b;            sprintf(sample,"%d",b);            strcat(question,sample);            c=a%10;            if (c<2) strcat(question,"*");            if ((c>=2)&&(c<6)) strcat(question,"+");            if ((c>=6)&&(c<=9)) strcat(question,"-");                }        a=rand();        b=a%9+1;        sprintf(sample,"%d",b);        strcat(question,sample);        fprintf(file,"%s %d\n",question,calculate(question));    }        //create level 3 question    for (k=0;k<90;k++){        strcpy(question,"");                for (i=0;i<4;i++){            a=rand();            b=a%9+1;            if (b<0) b=-b;            sprintf(sample,"%d",b);            strcat(question,sample);            c=a%10;            if (c<5) strcat(question,"*");            if ((c>=5)&&(c<8)) strcat(question,"+");            if ((c>=8)&&(c<=9)) strcat(question,"-");                }        a=rand();        b=a%9+1;        sprintf(sample,"%d",b);        strcat(question,sample);        fprintf(file,"%s %d \n",question,calculate(question));    }         fclose(file);    }int main(){    int a=6;    int b=2;    randomExpression("library.txt");}int prec(char symbol){        if(symbol== '(')        return 0;    if(symbol== ')')        return 0;    if(symbol=='+' || symbol=='-')        return 1;    if(symbol=='*' || symbol=='/')        return 2;        return 0;}