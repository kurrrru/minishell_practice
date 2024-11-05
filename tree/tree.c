/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 20:42:38 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/02 20:59:14 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

typedef enum e_token_type
{
	WORD,
	OPERATOR
}				t_token_type;

typedef struct s_node
{
	char			*str;
	t_token_type	type;
	struct s_node	*left;
	struct s_node	*right;
}				t_node;

void	*xmalloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	return (ptr);
}


t_node	*create_node(char *str, t_token_type type)
{
	t_node	*node;

	node = (t_node *)xmalloc(sizeof(t_node));
	node->str = str;
	node->type = type;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void	print_tree(t_node *node)
{
	if (!node)
		return ;
	printf("(");
	print_tree(node->left);
	printf("%s", node->str);
	print_tree(node->right);
	printf(")");
}

void	add_right(t_node **node, char *str, t_token_type type)
{
	t_node	*new_node;
	t_node	*tmp;

	new_node = create_node(str, type);
	if (!*node)
	{
		*node = new_node;
		return ;
	}
	if (type != WORD)
	{
		new_node->left = *node;
		*node = new_node;
	}
	else
	{
		tmp = *node;
		while (tmp->right)
			tmp = tmp->right;
		tmp->right = new_node;
	}
}

void	parse(t_node **node, char **str)
{
	while (**str)
	{
		if (**str == '(')
		{
			(*str)++;
			parse(node, str);
		}
		else if (**str == ')')
		{
			(*str)++;
			return ;
		}
		else if (**str == ' ')
			(*str)++;
		else if (**str == '+' || **str == '-' || **str == '*' || **str == '/')
		{
			add_right(node, *str, OPERATOR);
			(*str)++;
		}
		else
		{
			add_right(node, *str, WORD);
			while (**str && **str != ' ' && **str != '(' && **str != ')')
				(*str)++;
		}
	}
}
