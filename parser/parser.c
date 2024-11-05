/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 23:24:54 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/05 23:34:51 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static t_node	*new_node(t_token token)
{
	t_node	*node;

	node = (t_node *)xmalloc(sizeof(t_node));
	if (token.type == PIPE)
		node->type = NODE_PIPE;
	else if (token.type == LOGICAL_AND)
		node->type = NODE_LOGICAL_AND;
	else if (token.type == LOGICAL_OR)
		node->type = NODE_LOGICAL_OR;
	else
		node->type = NODE_COMMAND;
	node->command = NULL;
	node->redirect = NULL;
	node->argv = NULL;
	return (node);
}

void	parser(t_node **root, t_data *data, int i)
{
	t_node	*node;

	if (data->token_arr[i].type == PIPE || data->token_arr[i].type == LOGICAL_AND || data->token_arr[i].type == LOGICAL_OR)
	{
		node = new_node(data->token_arr[i]);
		node->left = *root;
		*root = node;
	}
	else
	{
		
	}
}