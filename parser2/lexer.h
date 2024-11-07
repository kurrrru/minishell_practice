/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 22:04:10 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/03 22:39:17 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef enum e_token_type
{
	WORD,
	PIPE,
	REDIRECT_IN,
	REDIRECT_OUT,
	REDIRECT_HEREDOC,
	REDIRECT_APPEND,
	PAREN_LEFT,
	PAREN_RIGHT,
	LOGICAL_AND,
	LOGICAL_OR,
}		t_token_type;

typedef struct s_token
{
	char			*token;
	t_token_type	type;
}		t_token;

typedef struct s_data
{
	t_token	*token_arr;
	int		token_num;
}		t_data;

void	lexer(const char *input_line, t_data *data);
void	assign_token_type(t_data *data);

#endif