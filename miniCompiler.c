#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum
{
    TOKEN_NUM,
    TOKEN_VAR,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    int value;
} Token;

typedef struct ASTNode
{
    TokenType op;
    int value;
    struct ASTNode *left, *right;
} ASTNode;

Token current;
const char *input;
int pos = 0;
int vars[2] = {0};

void next_token()
{
    while (isspace(input[pos]))
        pos++;

    if (input[pos] == '\0')
    {
        current.type = TOKEN_EOF;
        return;
    }

    char c = input[pos++];
    if (c == 'a' || c == 'b')
    {
        current.type = TOKEN_VAR;
        current.value = c;
    }
    else if (c == '+')
        current.type = TOKEN_PLUS;
    else if (c == '-')
        current.type = TOKEN_MINUS;
    else if (c == '*')
        current.type = TOKEN_MUL;
    else if (c == '/')
        current.type = TOKEN_DIV;
    else if (c == '(')
        current.type = TOKEN_LPAREN;
    else if (c == ')')
        current.type = TOKEN_RPAREN;
    else if (isdigit(c))
    {
        current.type = TOKEN_NUM;
        current.value = c;
        while (isdigit(c))
        {
            current.value = current.value * 10 +
                            (input[pos++] - '0');
        }
    }
    else
    {
        fprintf(stderr, "Error: Unexptected character %c\n", c);
        exit(1);
    }
}

ASTNode *parse_expr();
ASTNode *parse_term();
ASTNode *parse_factor();

ASTNode *parse_expr()
{
    ASTNode *node = parse_term();

    while (current.type == TOKEN_PLUS || current.type == TOKEN_MINUS)
    {
        ASTNode *op_node = malloc(sizeof(ASTNode));
        op_node->op = current.type;
        op_node->left = node;
        next_token();
        op_node->right = parse_term();
        node = op_node;
    }
    return node;
}

ASTNode *parse_term()
{
    ASTNode *node = parse_factor();
    while (current.type == TOKEN_MUL || current.type == TOKEN_DIV)
    {
        ASTNode *op_node = malloc(sizeof(ASTNode));
        op_node->op = current.type;
        op_node->left = node;
        next_token();
        op_node->right = parse_factor();
        node = op_node;
    }
    return node;
}

ASTNode *parse_factor()
{
    if (current.type == TOKEN_LPAREN)
    {
        next_token();
        ASTNode *node = parse_expr();
        if (current.type != TOKEN_RPAREN)
        {
            fprintf(stderr, "Error: Expected ')'\n");
            exit(1);
        }
        next_token();
        return node;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (current.type == TOKEN_VAR)
    {
        node->op = TOKEN_VAR;
        node->value = current.value;
        next_token();
    }
    else if (current.type == TOKEN_NUM)
    {
        node->op = TOKEN_NUM;
        node->value = current.value;
        next_token();
    }
    else
    {
        fprintf(stderr, "Error: Expected number or variable\n");
        exit(1);
    }
    return node;
}

int eval(ASTNode *node)
{
    if (node->op == TOKEN_VAR)
        return vars[node->value - 'a'];
    else if (node->op == TOKEN_NUM)
        return node->value;
    else if (node->op == TOKEN_PLUS)
        return eval(node->left) + eval(node->right);
    else if (node->op == TOKEN_MINUS)
        return eval(node->left) - eval(node->right);
    else if (node->op == TOKEN_MUL)
        return eval(node->left) * eval(node->right);
    else if (node->op == TOKEN_DIV)
    {
        if (eval(node->right) == 0)
        {
            fprintf(stderr, "Error: division by 0\n");
            exit(1);
        }
        return eval(node->left) / eval(node->right);
    }
    return 0; 
}

void free_ast(ASTNode *node)
{
    if (node)
    {
        free_ast(node->left);
        free_ast(node->right);
        free(node);
    }
}

int main()
{
    printf("Enter value for 'a': ");
    scanf("%d", &vars[0]);
    printf("Enter value for 'b': ");
    scanf("%d", &vars[1]);

    while (getchar() != '\n')
        ;

    printf("Enter expression (e.g., a*(b+2)): ");
    char expr[100];
    fgets(expr, sizeof(expr), stdin);

    input = expr;
    next_token();

    ASTNode *ast = parse_expr();
    if (current.type != TOKEN_EOF)
    {
        fprintf(stderr, "Error: Unexpected tokens at end\n");
        exit(1);
    }

    printf("Result: %d\n", eval(ast));
    free_ast(ast);
    return 0;
}
