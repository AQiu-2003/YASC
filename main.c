#include "main.h"
#include "parser.tab.h"

int nodeNum;
int hasFault;
tnode nodeList[5000];

Ast newAst(char *name, int num, ...) {
    // 生成父节点
    tnode father = (tnode) malloc(sizeof(struct treeNode));
    // 添加子节点
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    if (!father) {
        yyerror("create treenode error");
        exit(0);
    }
    father->name = name;

    // 参数列表，详见 stdarg.h 用法
    va_list list;
    // 初始化参数列表
    va_start(list, num);

    // 表示当前节点不是终结符号，还有子节点
    if (num > 0) {
        temp = va_arg(list, tnode);
        for (int i = 0; i < num; ++i) {
            temp->father = father;
            if (father->fchild == NULL) {
                father->fchild = temp;
                father->line = temp->line;
            }
            if (i < num - 1) {
                temp->next = va_arg(list, tnode);
                temp = temp->next;
            }
        }


//        // 第一个孩子节点
//        temp = va_arg(list, tnode);
//        setChildTag(temp);
//        father->fchild = temp;
//        // 父节点行号为第一个孩子节点的行号
//        father->line = temp->line;
//
//        // 多个节点，继续添加
//        if (num >= 2) {
//            for (i = 0; i < num - 1; ++i) {
//                temp->next = va_arg(list, tnode);
//                temp = temp->next;
//                // 该节点为其他节点的子节点
//                setChildTag(temp);
//            }
//        }
    } else { //表示当前节点是终结符（叶节点）或者空的语法单元，此时num表示行号（空单元为-1）,将对应的值存入union
        father->line = va_arg(list, int);
        // strcmp()==0 表示相同
        if ((!strcmp(name, "ID")) || (!strcmp(name, "TYPE"))) {
            char *str;
            str = (char *) malloc(sizeof(char) * 40);
            strcpy(str, yytext);
            father->id_type = str;
        } else if (!strcmp(name, "INTC")) {
            father->intval = atoi(yytext);
        } else if (!strcmp(name, "CHARC")) {
            father->charval = yytext[1];
        } else {
            father->id_type = NULL;
        }
    }
    return father;
}

// 父节点->左子节点->右子节点....
void Preorder(Ast ast, int level) {
    // printf(" into ");
    if (ast != NULL) {
        // 层级结构缩进
        for (int i = 0; i < level; ++i) {
            printf(" ");
        }
        // printf(" rline ");
        if (ast->line != -1) {
            // printf(" prnt ");
            // 打印节点类型
            printf("%s", ast->name);
            // 根据不同类型打印节点数据
            if ((!strcmp(ast->name, "ID")) || (!strcmp(ast->name, "TYPE"))) {
                printf(": %s", ast->id_type);
            } else if (!strcmp(ast->name, "INTC")) {
                printf(": %d", ast->intval);
            } else if (!strcmp(ast->name, "CHARC")) {
                printf(": %c", ast->charval);
            } else {
                // 非叶节点打印行号
                printf("(%d)", ast->line);
            }
        }
        printf("\n");
        // printf(" fchild ");
        Preorder(ast->fchild, level + 1);
        // printf(" next ");
        Preorder(ast->next, level);
    }
    // printf(" return ");
}

// 错误处理
void yyerror(char *msg) {
    hasFault = 1;
    fprintf(stderr, "Syntax Error [Line %d]: %s, before '%s'\n", yylineno, msg, yytext);
}

// 主函数 扫描文件并且分析
// 为bison会自己调用yylex()，所以在main函数中不需要再调用它了
// bison使用yyparse()进行语法分析，所以需要我们在main函数中调用yyparse()和yyrestart()
int main(int argc, char **argv) {
    int j;
    int printTree = 1;
    printf("YASC: Start analysis...\n");
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    int i = 1;
    if (argc > 2) {
        if(strcmp(argv[1], "--no-tree") == 0) {
            printTree = 0;
            i = 2;
        }
    }
    for (; i < argc; i++) {
        // 初始化节点记录列表
        nodeNum = 0;
        memset(nodeList, 0, sizeof(tnode) * 5000);
        hasFault = 0;

        FILE *f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[i]);
            return 1;
        }
        yyrestart(f);
        yyparse();
        fclose(f);

        // 遍历所有非子节点的节点
        if (hasFault)
            continue;
        else
            printf("YASC: No syntax error found.\n");
        if (printTree) {
            for (j = 0; j < nodeNum; j++) {
                if (nodeList[j]->father == NULL) {
                    Preorder(nodeList[j], 0);
                }
            }
        }

    }
    printf("YASC: Finish analysis!\n");
}
/**********************语义分析**************************/
// 分析语法树，建立符号表
void analysis(tnode val){
    int i;
    
}

var *varhead, *vartail;
// 建立变量符号
void newvar(int num,...){
    va_list valist;
    va_start(valist,num);
    var* res=(var*)malloc(sizeof(var));
    tnode temp=(tnode)malloc(sizeof(tnode));
    temp=va_arg(valist,tnode);//var type id;
    res->type=temp->content;
    temp=va_arg(valist,tnode);
    res->name=temp->content;
    vartail->next=res;
    vartail=res;
}
// 变量是否已经定义
int findvar(tnode val){
    var *temp=(var *)malloc(sizeof(var*));
    temp=varhead->next;
    while (temp!=NULL)
    {
        if(!strcmp(temp->name,val->name)){
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}
// 变量类型
char* typevar(tnode val){
    var* temp=(var*)malloc(sizeof(var *));
    temp=varhead->next;
    while (temp!=NULL)
    {
        if(!strcmp(temp->name,val->content)){
            return temp->type;
        }
        temp=temp->next;
    }
    return NULL;
}
// 这样赋值号左边仅能出现ID、Exp LB Exp RB 以及 Exp DOT ID
int checkleft(tnode val);

type *typehead,*typetail;
// 建立类型符号表
 void newtype(int num,...);
// 查询是否已经定义
 int findtype(tnode val);
 char *typetype(tnode val);

func *funchead,*functail;
// 记录函数实参
int va_num;
char* va_type[10];
// 函数实际返回值类型
char *rtype[10];
//void getargs(tnode Args);//获取实参
int checkrtype(tnode ID,tnode Args)//检查形参与实参是否一致
{
    int i;
    va_num=0;
    getretype(Args);
    func *temp=(func*)malloc(sizeof(func*));
    temp=funchead->next;
    while(temp!=NULL&&temp->name!=NULL&&temp->tag==1){
        if(!strcmp(temp->name,ID->content))
            break;
        temp=temp->next;
    }
    if(va_num!=temp->va_num)
        return 1;
    for(i=0;i<temp->va_num;i++){
        if(temp->va_type[i]==NULL||va_type[i]==NULL||strcmp(temp->va_type[i],va_type[i])!=0){
            return 1;
        }
    }
    return 0;
}
// 建立函数符号
void newfunc(int num, ...){
    int i;
    va_list valist;
    va_start(valist,num);
    tnode temp=(tnode)malloc(sizeof(struct treeNode));
    switch(num){
        case 1: //procedure A()
            temp=va_arg(valist,tnode);
            functail->name=temp->content;
            functail->tag=1;
            func *new=(func*)malloc(sizeof(func));
            functail->next=new;
            functail=new;
            break;
        case 2: // procedure A(prarmlist)
            temp=va_arg(valist,tnode);
            functail->name=temp->content;
            temp=va_arg(valist,tnode);
            functail->va_num=0;
            getdetype(temp);
            break;
        default:
            break;
    }
}
void getdetype(tnode val){//定义的参数
    int i;
    if(val!=NULL){
        if(!strcmp(val->name,"Param")){
            functail->va_type[functail->va_num]=val->content;
            functail->va_num++;
            return;
        }
        for(i=0;i<val->ncld;i++){
            getdetype(val->fchild);
        }
    }
    else
        return;
}
void getretype(tnode val)//实际的参数
{
    int i;
    if(val!=NULL){
        if(!strcmp(val->name,"ActParamList")){
            va_type[va_num]=val->content;
            va_num++;
        }
        for(i=0;i<val->ncld;i++){
            getretype(val->fchild);
        }
    }
}
// 函数是否已经定义
int findfunc(tnode val){
    func *temp=(func*)malloc(sizeof(func*));
    temp=funchead->next;
    while(temp!=NULL&&temp->name!=NULL&&temp->tag==1){
        if(!strcmp(temp->name,val->content)){
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}
// 函数的形参个数
int numfunc(tnode val){
    func *temp=(func*)malloc(sizeof(func*));
    temp=funchead->next;
    while (temp!=NULL)
    {
        if(!strcmp(temp->name,val->content)){
            return temp->va_num;
        }
        temp=temp->next;
    }
    
}


array *arrayhead,*arraytail;
// 建立数组符号
void newarray(int num, ...){
    va_list valist;
    va_start(valist,num);
    array *res=(array*)malloc(sizeof(array));
    tnode temp=(tnode)malloc(sizeof(struct treeNode));
    //array a[1..10] of basetype; a 1 10 type newarray(2,$2,$8)
    temp=va_arg(valist,tnode);
    res->name=temp->content;
    temp=va_arg(valist,tnode);
    res->low=atoi(temp->content);
    temp=va_arg(valist,tnode);
    res->top=atoi(temp->content);
    temp=va_arg(valist,tnode);
    res->type=temp->content;
    arraytail->next=res;
    arraytail=res;
}
// 查找数组是否已经定义
int findarray(tnode val){
    array *temp=(array*)malloc(sizeof(array *));
    temp=arrayhead->next;
    while (temp!=NULL)
    {
        /* code */
        if(!strcmp(temp->name,val->content))
            return 1;
        temp=temp->next;
    }
    return 0;
}
// 数组类型
char *typearray(tnode val){
    array *temp=(array *)malloc(sizeof(array*));
    temp=arrayhead->next;
    while (temp!=NULL)
    {
        /* code */
        while (temp!=NULL)
        {
            /* code */
            if(!strcmp(temp->name,val->content)){
                return temp->type;
            }
            temp=temp->next;
        }
        
    }
    return NULL;
}