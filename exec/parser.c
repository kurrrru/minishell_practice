/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 23:24:54 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/09 11:48:15 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "lexer.h"
#include <readline/readline.h>

static void	ft_bzero(void *s, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
		ptr[i++] = 0;
}

static void	*xmalloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	ft_bzero(ptr, size);
	return (ptr);
}

static char	*xstrndup(const char *s, size_t n)
{
	char	*new_s;
	size_t	i;

	new_s = (char *)xmalloc(n + 1);
	i = 0;
	while (i < n && s[i])
	{
		new_s[i] = s[i];
		i++;
	}
	new_s[i] = '\0';
	return (new_s);
}

static t_node *new_command_node(t_token token) 
{
    t_node *node = (t_node *)xmalloc(sizeof(t_node));
    node->type = NODE_COMMAND;
    node->command = xstrndup(token.token, strlen(token.token));
    node->redirect_num = 0;
    node->redirect_capacity = 2;
    node->redirect = (t_redirect *)xmalloc(sizeof(t_redirect) * node->redirect_capacity);
    node->arg_num = 0;
    node->arg_capacity = 2;
    node->argv = (char **)xmalloc(sizeof(char *) * node->arg_capacity);
    return (node);
}

static t_node *new_connector_node(t_token token) 
{
    t_node *node = (t_node *)xmalloc(sizeof(t_node));
    if (token.type == PIPE) 
        node->type = NODE_PIPE;
    else if (token.type == LOGICAL_AND)
        node->type = NODE_LOGICAL_AND;
    else if (token.type == LOGICAL_OR)
        node->type = NODE_LOGICAL_OR;
    return (node);
}

t_node *parse_data(t_data *data, int *index, int *paren_count, const int depth) // depthを追加
{
    t_node *root = NULL;
    
	// fprintf(stderr, "index: %d, token_num: %d\n", *index, data->token_num); // 追加 for debug
    while (*index < data->token_num)
    {
        t_token token = data->token_arr[*index];
        if (token.type == PIPE || token.type == LOGICAL_AND || token.type == LOGICAL_OR)
        {
			// fprintf(stderr, "depth value: %d, token: %s\n", depth, token.token); // 追加 for debug
			if ((token.type == LOGICAL_AND || token.type == LOGICAL_OR) && depth > 0) // 追加
			{
				(*index)--; // 辻褄合わせ
				return root;
			}
			t_node *conn_node = new_connector_node(token);
            conn_node->left = root;
            root = conn_node;
            (*index)++;
            //↓この行で、木が右下に伸びていっちゃってる。returnの条件を変えてやるのは難しそう
            conn_node->right = parse_data(data, index, paren_count, depth + 1); // depthを追加
        }
        else if (token.type == PAREN_LEFT)
        {
            (*paren_count)++;
            (*index)++;
            t_node *sub_tree = parse_data(data, index, paren_count, 0); // depthを追加
            // (*index)++; この行コメントアウトしたらうまくいくかも
            if (root == NULL)
                root = sub_tree;
            else
            {
                if (root->type == NODE_PIPE || root->type == NODE_LOGICAL_AND || root->type == NODE_LOGICAL_OR)
                {
                    t_node *conn_node = root;
                    conn_node->right = sub_tree;
                }
                else
                {
                    fprintf(stderr, "Syntax error: unexpected parentheses\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        else if (token.type == PAREN_RIGHT)
        {
            (*paren_count)--;
            if (*paren_count < 0)
            {
                fprintf(stderr, "Syntax error: unmatched ')'\n");
                exit(EXIT_FAILURE);
            }
            return root;
        }
        else if (token.type == REDIRECT_IN || token.type == REDIRECT_OUT ||
                 token.type == REDIRECT_APPEND || token.type == REDIRECT_HEREDOC)
        {
            if (root == NULL)
                root = new_command_node((t_token){.token = "", .type = WORD});
            (*index)++;
            if (*index < data->token_num && data->token_arr[*index].type == WORD)
            {
                t_token file_token = data->token_arr[*index];
                t_redirect redirect;

                if (token.type == REDIRECT_IN)
                    redirect.type = IN;
                else if (token.type == REDIRECT_OUT)
                    redirect.type = OUT;
                else if (token.type == REDIRECT_HEREDOC)
                    redirect.type = HEREDOC;
                else if (token.type == REDIRECT_APPEND)
                    redirect.type = APPEND;
                redirect.file = xstrndup(file_token.token, strlen(file_token.token));
                if (root->redirect_num >= root->redirect_capacity)
                {
                    root->redirect_capacity *= 2;
                    root->redirect = realloc(root->redirect, sizeof(t_redirect) * root->redirect_capacity);
                }
                root->redirect[root->redirect_num++] = redirect;
                (*index)++;
                if (*index < data->token_num && data->token_arr[*index].type == WORD)
                {
                    t_token command_token = data->token_arr[*index];
                    root->command = xstrndup(command_token.token, strlen(command_token.token));
                }
            }
            else
            {
                fprintf(stderr, "Syntax error: missing file for redirect\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (token.type == WORD)
        {
            if (root == NULL)
                root = new_command_node(token);
            else
            {
                t_node *cmd_node = root;
                if (cmd_node->arg_num >= cmd_node->arg_capacity)
                {
                    cmd_node->arg_capacity *= 2;
                    cmd_node->argv = realloc(cmd_node->argv, sizeof(char *) * cmd_node->arg_capacity);
                }
                cmd_node->argv[cmd_node->arg_num++] = xstrndup(token.token, strlen(token.token));
            }
        }
        (*index)++;
        //右上に木を伸ばしていく方法考えてる
        // token = data->token_arr[*index];
        // if ((token.type == PIPE || token.type == LOGICAL_AND || token.type == LOGICAL_OR) && *paren_count == 0)
        //     return root;
    }
    return root;
}

// 主にここから下が追加された部分
void parser(t_node **root, t_data *data)
{
    int index = 0;
    int paren_count = 0;

    *root = parse_data(data, &index, &paren_count, 0);
    if (paren_count != 0)
    {
        printf("Syntax error: unmatched '('\n");
        exit(EXIT_FAILURE);
    }
}

void	dump_node(t_node *node, int depth) 
{
	if (node == NULL)
		return;
	if (node->type == NODE_COMMAND) 
	{
		for (int i = 0; i < depth; i++)
			printf("  ");
		printf("command: %s\n", node->command);
		for (int i = 0; i < node->arg_num; i++) 
		{
			for (int j = 0; j < depth; j++)
				printf("  ");
			printf("arg: %s\n", node->argv[i]);
		}
		for (int i = 0; i < node->redirect_num; i++) 
		{
			for (int j = 0; j < depth; j++)
				printf("  ");
			printf("redirect: %d %s\n", node->redirect[i].type, node->redirect[i].file);
		}
		for (int i = 0; i < depth; i++)
			printf("  ");
		printf("child: %p, %p\n", node->left, node->right);
	}
	else 
	{
		char *connector[3] = {"PIPE", "LOGICAL_AND", "LOGICAL_OR"};
		dump_node(node->left, depth + 1);
		for (int i = 0; i < depth; i++)
			printf("  ");
		printf("connector: %s\n", connector[node->type]);
		for (int i = 0; i < depth; i++)
			printf("  ");
		printf("depth: %d\n", depth);
		dump_node(node->right, depth + 1);
	}
}

void dump_tree(t_node *root) 
{
	dump_node(root, 0);
}

void free_node(t_node *node) 
{
	if (node == NULL)
		return;
	if (node->type == NODE_COMMAND) 
	{
		free(node->command);
		for (int i = 0; i < node->arg_num; i++)
			free(node->argv[i]);
		free(node->argv);
		for (int i = 0; i < node->redirect_num; i++)
			free(node->redirect[i].file);
		free(node->redirect);
	}
	else 
	{
		free_node(node->left);
		free_node(node->right);
	}
	free(node);
}

void free_tree(t_node *root) 
{
	free_node(root);
}

// int main()
// {
// 	char *input_data;
// 	t_data data;
// 	t_node *root;

// 	while (1)
// 	{
// 		input_data = readline("parser$ ");
// 		if (!input_data)
// 			break;
// 		lexer(input_data, &data);
// 		assign_token_type(&data);
// 		parser(&root, &data);
// 		dump_tree(root);
// 		free_tree(root);
// 	}
// }
