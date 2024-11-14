/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 10:32:09 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/14 21:30:39 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include "parser.h"
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <sys/wait.h>
# include "config.h"

typedef struct s_exec
{
	char	*command;
	char	**argv;
	int		in_fd;
	int		out_fd;
	char	**envp;
}	t_exec;

int	run(t_node *node, int in_fd, int out_fd, t_config *config);
int	run_tree(t_node *root, int in_fd, int out_fd, t_config *config);

#endif
