/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 22:22:20 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/09 17:28:54 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*dst_ptr;
	unsigned char	*src_ptr;
	size_t			i;

	dst_ptr = (unsigned char *)dest;
	src_ptr = (unsigned char *)src;
	i = 0;
	while (i < n)
	{
		dst_ptr[i] = src_ptr[i];
		i++;
	}
	return (dest);
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

static void	*xrealloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;

	new_ptr = malloc(new_size);
	if (!new_ptr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	ft_memcpy(new_ptr, ptr, old_size);
	free(ptr);
	return (new_ptr);
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

static int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

static int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

static int is_space(char c)
{
	return (c == ' ' || c == '\t');
}

static int	sign_len(char *s)
{
	if (ft_strncmp(s, "&&", 2) == 0 || ft_strncmp(s, "||", 2) == 0 || ft_strncmp(s, ">>", 2) == 0 || ft_strncmp(s, "<<", 2) == 0)
		return (2);
	if (s[0] == '|' || s[0] == '<' || s[0] == '>' || s[0] == '(' || s[0] == ')')
		return (1);
	return (0);
}

void	lexer(const char *input_line, t_data *data)
{
	int		dquote;
	int		squote;
	int		sep;
	int		i;
	int		j;

	dquote = 0;
	squote = 0;
	sep = 1;
	i = -1;
	data->token_num = 0;
	while (input_line[++i])
	{
		if (input_line[i] == '"' && !squote)
		{
			if (!dquote && sep)
			{
				data->token_num++;
				sep = 0;
			}
			dquote = !dquote;
		}
		else if (input_line[i] == '\'' && !dquote)
		{
			if (!squote && sep)
			{
				data->token_num++;
				sep = 0;
			}
			squote = !squote;
		}
		else if (is_space(input_line[i]) && !dquote && !squote)
			sep = 1;
		else if (sign_len((char *)&input_line[i]) > 0 && !dquote && !squote)
		{
			data->token_num++;
			i += sign_len((char *)&input_line[i]) - 1;
			sep = 1;
		}
		else if (sep && !is_space(input_line[i]))
		{
			data->token_num++;
			sep = 0;
		}
	}
	printf("token_num: %d\n", data->token_num);
	if (squote || dquote)
	{
		write(STDERR_FILENO, "syntax error\n", 13);
		exit(EXIT_FAILURE);
	}
	data->token_arr = (t_token *)xmalloc(sizeof(t_token) * data->token_num + 1);
	i = -1;
	j = 0;
	while (input_line[++i])
	{
		if (!is_space(input_line[i]) && sign_len((char *)&input_line[i]) == 0)
		{
			input_line += i;
			i = -1;
			while (input_line[++i])
			{
				if (is_space(input_line[i]) && !dquote && !squote)
					break ;
				else if (sign_len((char *)&input_line[i]) > 0 && !dquote && !squote)
					break ;
				if (input_line[i] == '"' && !squote)
					dquote = !dquote;
				else if (input_line[i] == '\'' && !dquote)
					squote = !squote;
			}
			data->token_arr[j++].token = xstrndup(input_line, i);
			input_line += i;
			i = -1;
		}
		else if (sign_len((char *)&input_line[i]) > 0 && !dquote && !squote)
		{
			data->token_arr[j++].token = xstrndup((char *)&input_line[i], sign_len((char *)&input_line[i]));
			input_line += sign_len((char *)&input_line[i]) + i;
			i = -1;
		}
	}
}

void	assign_token_type(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->token_num)
	{
		if (ft_strcmp(data->token_arr[i].token, "|") == 0)
			data->token_arr[i].type = PIPE;
		else if (ft_strcmp(data->token_arr[i].token, "<") == 0)
			data->token_arr[i].type = REDIRECT_IN;
		else if (ft_strcmp(data->token_arr[i].token, ">") == 0)
			data->token_arr[i].type = REDIRECT_OUT;
		else if (ft_strcmp(data->token_arr[i].token, ">>") == 0)
			data->token_arr[i].type = REDIRECT_APPEND;
		else if (ft_strcmp(data->token_arr[i].token, "<<") == 0)
			data->token_arr[i].type = REDIRECT_HEREDOC;
		else if (ft_strcmp(data->token_arr[i].token, "(") == 0)
			data->token_arr[i].type = PAREN_LEFT;
		else if (ft_strcmp(data->token_arr[i].token, ")") == 0)
			data->token_arr[i].type = PAREN_RIGHT;
		else if (ft_strcmp(data->token_arr[i].token, "&&") == 0)
			data->token_arr[i].type = LOGICAL_AND;
		else if (ft_strcmp(data->token_arr[i].token, "||") == 0)
			data->token_arr[i].type = LOGICAL_OR;
		else
			data->token_arr[i].type = WORD;
	}
}

void	free_data(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->token_num)
		free(data->token_arr[i].token);
	free(data->token_arr);
}

// int	main(void)
// {
// 	char		*input_line;
// 	t_data		data;
// 	int			i;
// 	char		*type[] = {"WORD", "PIPE", "REDIRECT_IN", "REDIRECT_OUT", "REDIRECT_HEREDOC", "REDIRECT_APPEND", "PAREN_LEFT", "PAREN_RIGHT", "LOGICAL_AND", "LOGICAL_OR"};

// 	while (1)
// 	{
// 		input_line = readline("lexer$ ");
// 		if (!input_line)
// 			break ;
// 		lexer(input_line, &data);
// 		assign_token_type(&data);
// 		i = -1;
// 		while (++i < data.token_num)
// 		{
// 			printf("token: [%s]\n", data.token_arr[i].token);
// 			printf("type: %s\n", type[data.token_arr[i].type]);
// 		}
// 	}
// 	return (EXIT_SUCCESS);
// }
