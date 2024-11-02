/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 00:52:24 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/02 01:07:02 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char	*getenv_wrap(char *name)
{
	char	*value;

	value = getenv(name);
	if (!value)
	{
		write(STDERR_FILENO, "getenv failed\n", 14);
		exit(EXIT_FAILURE);
	}
	return (value);
}

// getenv
int	main(void)
{
	char	*path;
	char	*name;

	name = "PATH";
	path = getenv_wrap(name);
	printf("%s: %s\n", name, path);
	name = "HOME";
	path = getenv_wrap(name);
	printf("%s: %s\n", name, path);
	name = "USER";
	path = getenv_wrap(name);
	printf("%s: %s\n", name, path);
	name = "SHELL";
	path = getenv_wrap(name);
	printf("%s: %s\n", name, path);
	return (EXIT_SUCCESS);
}
