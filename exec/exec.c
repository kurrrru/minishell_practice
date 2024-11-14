/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkawaguc <nkawaguc@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:41:09 by nkawaguc          #+#    #+#             */
/*   Updated: 2024/11/14 21:59:02 by nkawaguc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>

static void	ft_bzero(void *s, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
		ptr[i++] = 0;
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

static char *ft_strdup(const char *s)
{
	char	*new_s;
	size_t	i;

	new_s = (char *)xmalloc(strlen(s) + 1);
	i = 0;
	while (s[i])
	{
		new_s[i] = s[i];
		i++;
	}
	new_s[i] = '\0';
	return (new_s);
}

static char **ft_split(const char *s, char c)
{
	char	**split;
	size_t	i;
	size_t	j;
	size_t	k;

	split = (char **)xmalloc(sizeof(char *) * (strlen(s) + 1));
	i = 0;
	j = 0;
	while (s[i])
	{
		if (s[i] == c)
		{
			i++;
			continue ;
		}
		k = i;
		while (s[i] && s[i] != c)
			i++;
		split[j] = xstrndup(s + k, i - k);
		j++;
	}
	split[j] = NULL;
	return (split);
}

static void	free_split(char **split)
{
	size_t	i;

	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

static void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;

	ptr = malloc(count * size);
	if (!ptr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	ft_bzero(ptr, count * size);
	return (ptr);
}

static char *ft_strjoin(const char *s1, const char *s2)
{
	char	*new_s;
	size_t	i;
	size_t	j;

	new_s = (char *)xmalloc(strlen(s1) + strlen(s2) + 1);
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

static char *get_path(char *command)
{
	char	*path_part;
	char	*path;
	char	*path_env;
	char	**path_list;
	int		i;

	path = NULL;
	if (strchr(command, '/'))
	{
		if (access(command, F_OK) == 0)
			return (ft_strdup(command));
		write(2, "command not found\n", 18);
		exit(127);
	}
	path_env = getenv("PATH");
	if (path_env)
		path_env = ft_strdup(path_env);
	else
		path_env = getcwd(NULL, 0);
	path_list = ft_split(path_env, ':');
	if (!path_list)
	{
		perror("ft_split");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (path_list[i])
	{
		path_part = ft_strjoin(path_list[i], "/");
		path = ft_strjoin(path_part, command);
		free(path_part);
		if (access(path, F_OK) == 0)
			break ;
		free(path);
		path = NULL;
		i++;
	}
	free_split(path_list);
	if (!path)
	{
		write(STDERR_FILENO, command, strlen(command));
		write(STDERR_FILENO, ": command not found\n", 20);
		exit(127);
	}
	free(path_env);
	return (path);
}

static void	redirect_handler(t_node *node, int *in_fd, int *out_fd)
{
	int heredoc_fd[2];
	heredoc_fd[0] = -1;
	heredoc_fd[1] = -1;
	for (int i = 0; i < node->redirect_num; i++)
	{
		if (node->redirect[i].type == HEREDOC)
		{
			if (heredoc_fd[0] != -1)
			{
				if (heredoc_fd[0] != 0)
					close(heredoc_fd[0]);
				if (heredoc_fd[1] != 1)
					close(heredoc_fd[1]);
			}
			if (pipe(heredoc_fd) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			while (1)
			{
				char *line = readline("> ");
				if (!line)
				{
					write(2, "bash: warning: here-document delimited by end-of-file (wanted `", 61);
					write(2, node->redirect[i].file, strlen(node->redirect[i].file));
					write(2, "')\n", 3);
					break ;
				}
				if (strcmp(line, node->redirect[i].file) == 0)
				{
					free(line);
					break ;
				}
				write(heredoc_fd[1], line, strlen(line));
				write(heredoc_fd[1], "\n", 1);
				free(line);
			}
		}
	}
	if (heredoc_fd[0] != -1)
	{
		if (*in_fd != 0)
			close(*in_fd);
		*in_fd = heredoc_fd[0];
		if (heredoc_fd[1] != 1)
			close(heredoc_fd[1]);
	}
	for (int i = 0; i < node->redirect_num; i++)
	{
		if (node->redirect[i].type == IN)
		{
			if (*in_fd != 0)
				close(*in_fd);
			*in_fd = open(node->redirect[i].file, O_RDONLY);
			if (*in_fd == -1)
			{
				perror("open");
				exit(EXIT_FAILURE);
			}
		}
		else if (node->redirect[i].type == OUT)
		{
			if (*out_fd != 1)
				close(*out_fd);
			*out_fd = open(node->redirect[i].file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (*out_fd == -1)
			{
				perror("open");
				exit(EXIT_FAILURE);
			}
		}
		else if (node->redirect[i].type == APPEND)
		{
			if (*out_fd != 1)
				close(*out_fd);
			*out_fd = open(node->redirect[i].file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (*out_fd == -1)
			{
				perror("open");
				exit(EXIT_FAILURE);
			}
		}
	}
}

int	run(t_node *node, int in_fd, int out_fd, t_config *config)
{
	t_exec	exec;

	if (!node)
	{
		perror("node is NULL");
		exit(EXIT_FAILURE);
	}
	if (node->type == NODE_LOGICAL_AND)
	{
		redirect_handler(node, &in_fd, &out_fd);
		config->exit_status = run_tree(node->left, in_fd, out_fd, config);
		if (config->exit_status == 0)
			return (run_tree(node->right, in_fd, out_fd, config));
		return (config->exit_status);
	}
	else if (node->type == NODE_LOGICAL_OR)
	{
		redirect_handler(node, &in_fd, &out_fd);
		config->exit_status = run_tree(node->left, in_fd, out_fd, config);
		if (config->exit_status != 0)
			config->exit_status = run_tree(node->right, in_fd, out_fd, config);
		return (config->exit_status);
	}
	else if (node->type == NODE_PIPE)
	{
		int	pipe_fd[2];
		int status;

		redirect_handler(node, &in_fd, &out_fd);
		if (pipe(pipe_fd) == -1)
		{
			perror("pipe");
			exit(EXIT_FAILURE);
		}
		if (!node->left)
		{
			write(2, "left is NULL\n", 13);
			exit(EXIT_FAILURE);
		}
		if (!node->right)
		{
			write(2, "right is NULL\n", 14);
			exit(EXIT_FAILURE);
		}
		int pid1, pid2;
		if (node->left->type == NODE_COMMAND)
		{
			pid1 = fork();
			if (pid1 == -1)
			{
				perror("fork");
				exit(EXIT_FAILURE);
			}
			if (pid1 == 0)
			{
				close(pipe_fd[0]);
				if (out_fd != 1)
					close(out_fd);
				run(node->left, in_fd, pipe_fd[1], config);
			}
			else
			{
				close(pipe_fd[1]);
				if (node->right->type == NODE_COMMAND)
				{
					pid2 = fork();
					if (pid2 == -1)
					{
						perror("fork");
						exit(EXIT_FAILURE);
					}
					if (pid2 == 0)
					{
						if (in_fd != 0)
							close(in_fd);
						run(node->right, pipe_fd[0], out_fd, config);
					}
					else
					{
						if (in_fd != 0)
							close(in_fd);
						close(pipe_fd[0]);
						waitpid(pid1, &status, 0);
						waitpid(pid2, &status, 0);
					}
				}
				else
				{
					if (in_fd != 0)
						close(in_fd);
					int status2;
					status = run(node->right, pipe_fd[0], out_fd, config);
					waitpid(pid1, &status2, 0);
					close(pipe_fd[0]);
				}
			}
		}
		else
		{
			if (node->right->type == NODE_COMMAND)
			{
				pid2 = fork();
				if (pid2 == -1)
				{
					perror("fork");
					exit(EXIT_FAILURE);
				}
				if (pid2 == 0)
				{
					close(pipe_fd[1]);
					if (in_fd != 0)
						close(in_fd);
					run(node->right, pipe_fd[0], out_fd, config);
				}
				else
				{
					close(pipe_fd[0]);
					if (out_fd != 1)
						close(out_fd);
					status = run(node->left, in_fd, pipe_fd[1], config);
					close(pipe_fd[1]);
					waitpid(pid2, &status, 0);
				}
			}
			else
			{
				status = run(node->left, in_fd, pipe_fd[1], config);
				close(pipe_fd[1]);
				status = run(node->right, pipe_fd[0], out_fd, config);
				close(pipe_fd[0]);
			}
		}
		return (WEXITSTATUS(status));
	}
	exec.in_fd = in_fd;
	exec.out_fd = out_fd;
	redirect_handler(node, &exec.in_fd, &exec.out_fd);
	if (exec.in_fd != 0)
	{
		dup2(exec.in_fd, 0);
		close(exec.in_fd);
	}
	if (exec.out_fd != 1)
	{
		dup2(exec.out_fd, 1);
		close(exec.out_fd);
	}
	exec.command = get_path(node->command);
	exec.argv = ft_calloc(node->arg_num + 2, sizeof(char *));
	exec.argv[0] = exec.command;
	for (int i = 0; i < node->arg_num; i++)
		exec.argv[i + 1] = node->argv[i];
	exec.argv[node->arg_num + 1] = NULL;
	exec.envp = NULL;
	if (config->envp_num > 0)
	{
		exec.envp = make_envp(config);
		if (!exec.envp)
		{
			perror("malloc");
			exit(EXIT_FAILURE);
		}
	}
	execve(exec.command, exec.argv, exec.envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

int	run_tree(t_node *root, int in_fd, int out_fd, t_config *config)
{
	if (!root)
	{
		write(2, "root is NULL\n", 13);
		return (EXIT_SUCCESS);
	}
	int pid;
	if (root->type == NODE_COMMAND)
	{
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0)
		{
			run(root, in_fd, out_fd, config);
		}
		else
		{
			int status;
			waitpid(pid, &status, 0);
			return (WEXITSTATUS(status));
		}
	}
	return (run(root, in_fd, out_fd, config));
}

int main(int argc, char **argv, char **envp)
{
	char *input_data;
	t_data data;
	t_node *root;
	t_config config;

	(void)argc;
	(void)argv;
	if (init_config(&config, envp) == EXIT_FAILURE)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		if (isatty(STDERR_FILENO))
			input_data = readline("minishell$ ");
		else
			input_data = readline(NULL);
		add_history(input_data);
		if (!input_data)
		{
			if (isatty(STDERR_FILENO))
				write(2, "exit\n", 5);
			break;
		}
		config.exit_status = lexer(input_data, &data);
		free(input_data);
		if (config.exit_status != 0)
			continue ;
		assign_token_type(&data);
		parser(&root, &data, &config);
		free_data(&data);
		if (config.exit_status != EXIT_SUCCESS)
			continue ;
		dump_tree(root);
		config.exit_status = run_tree(root, 0, 1, &config);
		free_tree(root);
		printf("exit status: %d\n", config.exit_status);
	}
	free_config(&config);
}
