/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 22:17:42 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/14 22:18:17 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"
#include <string.h>

char	*findenv(const char *name, t_config *config)
{
	int		i;

	i = 0;
	while (i < config->envp_num)
	{
		if (strcmp(name, config->envp[i].key) == 0)
			return (config->envp[i].value);
		i++;
	}
	return (NULL);
}
