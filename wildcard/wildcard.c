/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 00:38:14 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/02 00:44:03 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

int	ft_strlen(char *str)
{
	int	len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
		ptr[i++] = 0;
}

void	*malloc_wrap(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	ft_bzero(ptr, size);
	return (ptr);
}

int	is_match(char *text, char *pattern)
{
	int	m;
	int	n;
	int	i;
	int	j;
	int	**dp;

	m = ft_strlen(text);
	n = ft_strlen(pattern);
	dp = malloc_wrap(sizeof(int *) * (m + 1));
	i = -1;
	while (++i <= m)
		dp[i] = malloc_wrap(sizeof(int) * (n + 1));
	dp[0][0] = TRUE;
	i = 0;
	while (++i <= n)
	{
		if (pattern[i - 1] == '*')
			dp[0][i] = dp[0][i - 1];
	}
	i = 0;
	while (++i <= m)
	{
		j = 0;
		while (++j <= n)
		{
			if (pattern[j - 1] == '*')
				dp[i][j] = dp[i][j - 1] || dp[i - 1][j];
			else if (pattern[j - 1] == '?' || text[i - 1] == pattern[j - 1])
				dp[i][j] = dp[i - 1][j - 1];
		}
	}
	return (dp[m][n]);
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s text pattern\n", argv[0]);
		return (EXIT_FAILURE);
	}
	if (is_match(argv[1], argv[2]))
		printf("Match\n");
	else
		printf("Not match\n");
	return (EXIT_SUCCESS);
}
