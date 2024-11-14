/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 22:04:10 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/14 20:07:19 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include "config.h"

# define SUCCESS 0
# define FAILURE 1
# define EXIT_INVALID_INPUT 2
# define EXIT_EXEC_ERROR 126
# define EXIT_CMD_NOT_FOUND 127

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

int		lexer(const char *input_line, t_data *data);
void	assign_token_type(t_data *data);
void	free_data(t_data *data);

#endif