/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 01:42:51 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/15 15:01:10 by nkawaguc         ###   ########.fr       */
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

static char	*ft_strndup(const char *s, size_t n)
{
	char	*new_s;
	size_t	i;

	new_s = (char *)malloc(n + 1);
	if (!new_s)
		return (NULL);
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
	t_node *node = (t_node *)malloc(sizeof(t_node));
	if (node == NULL)
	{
		perror("malloc");
		return (NULL);
	}
	node->type = NODE_COMMAND;
	if (token.token == NULL)
		node->command = NULL;
	else
	{
		node->command = ft_strndup(token.token, strlen(token.token));
		if (node->command == NULL)
		{
			perror("malloc");
			free(node);
			return (NULL);
		}
	}
	node->redirect_num = 0;
	node->redirect_capacity = 2;
	node->redirect = (t_redirect *)malloc(sizeof(t_redirect) * node->redirect_capacity);
	if (node->redirect == NULL)
	{
		perror("malloc");
		free(node->command);
		free(node);
		return (NULL);
	}
	node->arg_num = 0;
	node->arg_capacity = 2;
	node->argv = (char **)malloc(sizeof(char *) * node->arg_capacity);
	if (node->argv == NULL)
	{
		perror("malloc");
		free(node->command);
		free(node->redirect);
		free(node);
		return (NULL);
	}
	ft_bzero(node->argv, sizeof(char *) * node->arg_capacity);
	node->left = NULL;
	node->right = NULL;
	return (node);
}

static t_node *new_connector_node(t_token token)
{
	t_node *node = (t_node *)malloc(sizeof(t_node));
	if (node == NULL)
	{
		perror("malloc");
		return (NULL);
	}
	if (token.type == PIPE)
		node->type = NODE_PIPE;
	else if (token.type == LOGICAL_AND)
		node->type = NODE_LOGICAL_AND;
	else if (token.type == LOGICAL_OR)
		node->type = NODE_LOGICAL_OR;
	node->left = NULL;
	node->right = NULL;
	node->redirect_num = 0;
	node->redirect_capacity = 2;
	node->redirect = (t_redirect *)malloc(sizeof(t_redirect) * node->redirect_capacity);
	if (node->redirect == NULL)
	{
		perror("malloc");
		free(node);
		return (NULL);
	}
	node->command = NULL;
	node->argv = NULL;
	return (node);
}

// t_node *parse_data(t_data *data, int *index, int *paren_open, const int depth) // depthを追加
t_node *parse_data(t_data *data, t_parse_helper *ps, t_config *config, const int depth) // depthを追加
{
	t_node *root = NULL;

	while (ps->index < data->token_num)
	{
		t_token token = data->token_arr[ps->index];
		if (token.type == PIPE || token.type == LOGICAL_AND || token.type == LOGICAL_OR)
		{
			if ((token.type == LOGICAL_AND || token.type == LOGICAL_OR) && depth > 0)
			{
				ps->index--;
				return root;
			}
			t_node *conn_node = new_connector_node(token);
			if (conn_node == NULL)
			{
				config->exit_status = EXIT_FAILURE;
				free_tree(root);
				return (NULL);
			}
			conn_node->left = root;
			root = conn_node;
			ps->index++;
			conn_node->right = parse_data(data, ps, config, depth + 1);
			if (conn_node->right == NULL)
			{
				if (config->exit_status == EXIT_SUCCESS)
				{
					fprintf(stderr, "Syntax error: missing command after '%s'\n", token.token);
					config->exit_status = EXIT_INVALID_INPUT;
				}
				free_tree(root);
				return (NULL);
			}
		}
		else if (token.type == PAREN_LEFT)
		{
			if (ps->index > 0 && data->token_arr[ps->index - 1].type == PAREN_RIGHT)
			{
				fprintf(stderr, "Syntax error: unexpected word after ')'\n");
				config->exit_status = EXIT_INVALID_INPUT;
				free_tree(root);
				return (NULL);
			}
			ps->paren_open++;
			ps->index++;
			t_node *sub_tree = parse_data(data, ps, config, 0);
			if (sub_tree == NULL)
			{
				free_tree(root);
				return (NULL);
			}
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
					config->exit_status = EXIT_INVALID_INPUT;
					free_tree(root);
					return (NULL);
				}
			}
		}

		else if (token.type == PAREN_RIGHT)
		{
			ps->paren_open--;
			if (ps->paren_open < 0)
			{
				fprintf(stderr, "Syntax error: unmatched ')'\n");
				config->exit_status = EXIT_INVALID_INPUT;
				free_tree(root);
				return (NULL);
			}
			if (ps->index > 0 && data->token_arr[ps->index - 1].type == PAREN_LEFT)
			{
				fprintf(stderr, "Syntax error: unexpected ')'\n");
				config->exit_status = EXIT_INVALID_INPUT;
				free_tree(root);
				return (NULL);
			}
			return root;
		}
		else if (token.type == REDIRECT_IN || token.type == REDIRECT_OUT ||
					token.type == REDIRECT_APPEND || token.type == REDIRECT_HEREDOC)
		{
			if (root == NULL)
			{
				root = new_command_node((t_token){.token = NULL, .type = WORD});
				if (root == NULL)
				{
					config->exit_status = EXIT_FAILURE;
					return (NULL);
				}
			}
			ps->index++;
			if (ps->index < data->token_num && data->token_arr[ps->index].type == WORD)
			{
				t_token file_token = data->token_arr[ps->index];
				t_redirect redirect;

				if (token.type == REDIRECT_IN)
					redirect.type = IN;
				else if (token.type == REDIRECT_OUT)
					redirect.type = OUT;
				else if (token.type == REDIRECT_HEREDOC)
					redirect.type = HEREDOC;
				else if (token.type == REDIRECT_APPEND)
					redirect.type = APPEND;
				redirect.file = ft_strndup(file_token.token, strlen(file_token.token));
				if (redirect.file == NULL)
				{
					perror("malloc");
					config->exit_status = EXIT_FAILURE;
					free_tree(root);
					return (NULL);
				}
				if (root->redirect_num >= root->redirect_capacity)
				{
					root->redirect_capacity *= 2;
					root->redirect = realloc(root->redirect, sizeof(t_redirect) * root->redirect_capacity);
					if (root->redirect == NULL)
					{
						perror("malloc");
						config->exit_status = EXIT_FAILURE;
						free_tree(root);
						return (NULL);
					}
				}
				root->redirect[root->redirect_num++] = redirect;
			}
			else
			{
				fprintf(stderr, "Syntax error: missing file for redirect\n");
				config->exit_status = EXIT_INVALID_INPUT;
				free_tree(root);
				return (NULL);
			}
		}
		else if (token.type == WORD)
		{
			if (ps->index > 0 && data->token_arr[ps->index - 1].type == PAREN_RIGHT)
			{
				fprintf(stderr, "Syntax error: unexpected word after ')'\n");
				config->exit_status = EXIT_INVALID_INPUT;
				free_tree(root);
				return (NULL);
			}
			if (root == NULL)
			{
				root = new_command_node(token);
				if (root == NULL)
				{
					config->exit_status = EXIT_FAILURE;
					return (NULL);
				}
			}
			else
			{
				t_node *cmd_node = root;
				if (cmd_node->command == NULL)
				{
					cmd_node->command = ft_strndup(token.token, strlen(token.token));
					if (cmd_node->command == NULL)
					{
						perror("malloc");
						config->exit_status = EXIT_FAILURE;
						free_tree(root);
						return (NULL);
					}
				}
				else
				{
					if (cmd_node->arg_num >= cmd_node->arg_capacity)
					{
						cmd_node->arg_capacity *= 2;
						cmd_node->argv = realloc(cmd_node->argv, sizeof(char *) * cmd_node->arg_capacity);
					}
					cmd_node->argv[cmd_node->arg_num++] = ft_strndup(token.token, strlen(token.token));
					if (cmd_node->argv[cmd_node->arg_num - 1] == NULL)
					{
						perror("malloc");
						config->exit_status = EXIT_FAILURE;
						free_tree(root);
						return (NULL);
					}
				}
			}
		}
		ps->index++;
	}
	return root;
}

// 主にここから下が追加された部分
// if parser is successful, return 0 and exit_status is not changed
void parser(t_node **root, t_data *data, t_config *config)
{
	t_parse_helper	helper;

	helper.index = 0;
	helper.paren_open = 0;
	config->exit_status = EXIT_SUCCESS;
	*root = parse_data(data, &helper, config, 0);
	if (*root == NULL && config->exit_status != EXIT_SUCCESS)
		return ;
	if (helper.paren_open != 0)
	{
		printf("Syntax error: unmatched '('\n");
		config->exit_status = EXIT_INVALID_INPUT;
		return ;
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
		for (int i = 0; i < node->redirect_num; i++)
		{
			for (int j = 0; j < depth; j++)
				printf("  ");
			printf("redirect: %d %s\n", node->redirect[i].type, node->redirect[i].file);
		}
		dump_node(node->right, depth + 1);
	}
}

void dump_tree(t_node *root)
{
	dump_node(root, 0);
}

void free_node(t_node *node)
{
	int	i;

	if (node == NULL)
		return;
	free(node->command);
	i = -1;
	while (++i < node->arg_num)
		free(node->argv[i]);
	free(node->argv);
	i = -1;
	while (++i < node->redirect_num)
		free(node->redirect[i].file);
	free(node->redirect);
	free(node);
}

void free_tree(t_node *root)
{
	if (root == NULL)
		return;
	free_tree(root->left);
	free_tree(root->right);
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
