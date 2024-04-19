#include "main.h"
#include "parser.tab.h"

int nodeNum;
int hasFault;
tnode nodeList[5000];
tnode programNode;

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
    father->line = -1;

    // 参数列表，详见 stdarg.h 用法
    va_list list;
    // 初始化参数列表
    va_start(list, num);

    // 表示当前节点不是终结符号，还有子节点
    if (num > 0) {
        temp = va_arg(list, tnode);
        // father->line = temp->line;
        father->childCount = num;
        if (num == 1 && temp->content != NULL) {
            father->content = temp->content;
            father->type = temp->type;
            father->intvalue = temp->intvalue;
            father->charvalue = temp->charvalue;
            father->line = temp->line;
        } else {
            for (int i = 0; i < num; ++i) {
                temp->father = father;
                father->childs[i] = temp;
                temp = va_arg(list, tnode);
            }
            for (int i = 0; i < num; i++) {
                if (father->childs[i]->line != -1) {
                    father->line = father->childs[i]->line;
                    break;
                }
            }
        }


    } else { //表示当前节点是终结符（叶节点）或者空的语法单元，此时num表示行号（空单元为-1）,将对应的值存入union
        father->line = va_arg(list, int);
        father->content = NULL;
        // strcmp()==0 表示相同
        if (!strcmp(name, "INTC")) {
            father->intvalue = atoi(yytext);
            father->type = "int";
        } else if (!strcmp(name, "CHARC")) {
            father->charvalue = yytext[1];
            father->type = "char";
        } else {
            char *str;
            str = (char *) malloc(sizeof(char) * 40);
            strcpy(str, yytext);
            father->content = str;
            if (!strcmp(name, "ID")) father->type = "id";
            else if (!strcmp(name, "TYPE")) father->type = "type";
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
        // printf(" prnt ");
        // 打印节点类型
        printf("%s", ast->name);
        // 根据不同类型打印节点数据
        if (ast->type != NULL) {
            printf(": %s", ast->type);
            if (!strcmp(ast->type, "int")) printf(" -> %d", ast->intvalue);
            else if (!strcmp(ast->type, "char")) printf(" -> %c", ast->charvalue);
            else if (!strcmp(ast->type, "id") || !strcmp(ast->type, "type")) printf(" -> %s", ast->content);
        } else {
            printf("(%d)", ast->line);
        }

        printf("\n");
        for (int i = 0; i < ast->childCount; i++) {
            Preorder(ast->childs[i], level + 1);
        }
    }
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
        if (strcmp(argv[1], "--no-tree") == 0) {
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
            Preorder(programNode, 0);
        }

    }
    printf("YASC: Finish analysis!\n");
}
/**********************语义分析**************************/
// 分析语法树，建立符号表
void analysis(tnode val) {
    int i;

}

var *varhead, *vartail;

// 建立变量符号
void newvar(int num, ...) {
    va_list valist;
    va_start(valist, num);
    var *res = (var *) malloc(sizeof(var));
    tnode temp = (tnode) malloc(sizeof(tnode));
    temp = va_arg(valist, tnode);//var type id;
    res->type = temp->content;
    temp = va_arg(valist, tnode);
    res->name = temp->content;
    vartail->next = res;
    vartail = res;
}

// 变量是否已经定义
int findvar(tnode val) {
    var *temp = (var *) malloc(sizeof(var *));
    temp = varhead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->content)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// 变量类型
char *typevar(tnode val) {
    var *temp = (var *) malloc(sizeof(var *));
    temp = varhead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->content)) {
            return temp->type;
        }
        temp = temp->next;
    }
    return NULL;
}

// 这样赋值号左边仅能出现ID、Exp LB Exp RB 以及 Exp DOT ID
int checkleft(tnode val);

type *typehead, *typetail;

// 建立类型符号表
void newtype(int num, ...){
    va_list valist;
    va_start(valist,num);
    type* res=(type*)malloc(sizeof(type));
    tnode temp=(tnode)malloc(sizeof(tnode));
    temp=va_arg(valist,tnode);
    res->type=temp->content;
    temp=va_arg(valist,tnode);
    res->name=res;

    typetail->next=res;
    typetail=res;
 }
// 查询是否已经定义
int findtype(tnode val){

type *temp=(type *)malloc(sizeof(type*));
    temp=varhead->next;
    while (temp!=NULL)
    {
        if(!strcmp(temp->name,val->content)){
            return 1;
        }
        temp=temp->next;
    }
    while (temp!=NULL)
    {
        if(!strcmp(temp->type,val->content)){
            return 1;
        }
        temp=temp->next;
    }
    return 0;
 }
 char *typetype(tnode val){
    type* temp=(type*)malloc(sizeof(type*));
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

func *funchead, *functail;
// 记录函数实参
int va_num;
char *va_type[10];
// 函数实际返回值类型
char *rtype[10];

//void getargs(tnode Args);//获取实参
int checkrtype(tnode ID, tnode Args)//检查形参与实参是否一致
{
    int i;
    va_num = 0;
    getretype(Args);
    func *temp = (func *) malloc(sizeof(func *));
    temp = funchead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1) {
        if (!strcmp(temp->name, ID->content))
            break;
        temp = temp->next;
    }
    if (va_num != temp->va_num)
        return 1;
    for (i = 0; i < temp->va_num; i++) {
        if (temp->va_type[i] == NULL || va_type[i] == NULL || strcmp(temp->va_type[i], va_type[i]) != 0) {
            return 1;
        }
    }
    return 0;
}

// 建立函数符号
void newfunc(int num, ...) {
    int i;
    va_list valist;
    va_start(valist, num);
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    switch (num) {
        case 1: //procedure A()
            temp = va_arg(valist, tnode);
            functail->name = temp->content;
            functail->tag = 1;
            func *new = (func *) malloc(sizeof(func));
            functail->next = new;
            functail = new;
            break;
        case 2: // procedure A(prarmlist)
            temp = va_arg(valist, tnode);
            functail->name = temp->content;
            temp = va_arg(valist, tnode);
            functail->va_num = 0;
            getdetype(temp);
            break;
        default:
            break;
    }
}

void getdetype(tnode val) {//定义的参数
    int i;
    if (val != NULL) {
        if (!strcmp(val->name, "Param")) {
            functail->va_type[functail->va_num] = val->content;
            functail->va_num++;
            return;
        }
        for (i = 0; i < val->childCount; i++) {
            getdetype(val->childs[i]);
        }
    } else
        return;
}

void getretype(tnode val)//实际的参数
{
    int i;
    if (val != NULL) {
        if (!strcmp(val->name, "ActParamList")) {
            va_type[va_num] = val->content;
            va_num++;
        }
        for (i = 0; i < val->childCount; i++) {
            getretype(val->childs[i]);
        }
    }
}

// 函数是否已经定义
int findfunc(tnode val) {
    func *temp = (func *) malloc(sizeof(func *));
    temp = funchead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1) {
        if (!strcmp(temp->name, val->content)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// 函数的形参个数
int numfunc(tnode val) {
    func *temp = (func *) malloc(sizeof(func *));
    temp = funchead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->content)) {
            return temp->va_num;
        }
        temp = temp->next;
    }

}


array *arrayhead, *arraytail;

// 建立数组符号
void newarray(int num, ...) {
    va_list valist;
    va_start(valist, num);
    array *res = (array *) malloc(sizeof(array));
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    //array a[1..10] of basetype; a 1 10 type newarray(2,$2,$8)
    temp = va_arg(valist, tnode);
    res->name = temp->content;
    temp = va_arg(valist, tnode);
    res->low = atoi(temp->content);
    temp = va_arg(valist, tnode);
    res->top = atoi(temp->content);
    temp = va_arg(valist, tnode);
    res->type = temp->content;
    arraytail->next = res;
    arraytail = res;
}

// 查找数组是否已经定义
int findarray(tnode val) {
    array *temp = (array *) malloc(sizeof(array *));
    temp = arrayhead->next;
    while (temp != NULL) {
        /* code */
        if (!strcmp(temp->name, val->content))
            return 1;
        temp = temp->next;
    }
    return 0;
}

// 数组类型
char *typearray(tnode val) {
    array *temp = (array *) malloc(sizeof(array *));
    temp = arrayhead->next;
    while (temp != NULL) {
        /* code */
        while (temp != NULL) {
            /* code */
            if (!strcmp(temp->name, val->content)) {
                return temp->type;
            }
            temp = temp->next;
        }

    }
    return NULL;
}