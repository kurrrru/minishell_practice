/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 19:55:28 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/14 21:30:14 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
# define CONFIG_H

# include <stdlib.h>
# include <string.h>

typedef struct s_env
{
	char	*key;
	char	*value;
}	t_env;

typedef struct s_config
{
	int		exit_status;
	t_env	*envp;
	int		envp_num;
	int		envp_capacity;
}	t_config;

int		init_config(t_config *config, char **envp);
void	free_config(t_config *config);
char	**make_envp(t_config *config);

#endif
