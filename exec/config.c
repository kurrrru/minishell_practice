/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 20:51:28 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/14 21:31:46 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"

static char *ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;

	substr = (char *)malloc(len + 1);
	if (substr == NULL)
		return (NULL);
	i = 0;
	while (i < len && s[start + i])
	{
		substr[i] = s[start + i];
		i++;
	}
	substr[i] = '\0';
	return (substr);
}

int	init_config(t_config *config, char **envp)
{
	int	i;

	config->exit_status = 0;
	i = 0;
	while (envp[i])
		i++;
	config->envp = (t_env *)malloc(sizeof(t_env) * (i + 1));
	if (config->envp == NULL)
		return (EXIT_FAILURE);
	config->envp_capacity = i + 1;
	config->envp_num = 0;
	i = 0;
	while (envp[i])
	{
		char *equal = strchr(envp[i], '=');
		if (equal == NULL)
			continue ;
		config->envp[i].key = ft_substr(envp[i], 0, equal - envp[i]);
		if (config->envp[i].key == NULL)
		{
			free(config);
			return (EXIT_FAILURE);
		}
		config->envp[i].value = ft_substr(envp[i], equal - envp[i] + 1, strlen(envp[i]) - (equal - envp[i] + 1));
		if (config->envp[i].value == NULL)
		{
			free(config->envp[i].key);
			free(config);
			return (EXIT_FAILURE);
		}
		config->envp_num++;
		i++;
	}
}

void	free_config(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->envp_num)
	{
		free(config->envp[i].key);
		free(config->envp[i].value);
		i++;
	}
	free(config->envp);
}

static void	free_2d(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

static char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*new_s;
	size_t	i;
	size_t	j;

	new_s = (char *)malloc(strlen(s1) + strlen(s2) + 1);
	if (new_s == NULL)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		new_s[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		new_s[i + j] = s2[j];
		j++;
	}
	new_s[i + j] = '\0';
	return (new_s);
}

char	**make_envp(t_config *config)
{
	char	**envp;
	int		i;

	envp = (char **)malloc(sizeof(char *) * (config->envp_num + 1));
	if (envp == NULL)
		return (NULL);
	i = 0;
	while (i < config->envp_num)
	{
		char	*key_value;
		key_value = ft_strjoin(config->envp[i].key, "=");
		if (key_value == NULL)
		{
			free_2d(envp);
			return (NULL);
		}
		envp[i] = ft_strjoin(key_value, config->envp[i].value);
		free(key_value);
		if (envp[i] == NULL)
		{
			free_2d(envp);
			return (NULL);
		}
		i++;
	}
	envp[i] = NULL;
	return (envp);
}
