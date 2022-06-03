/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 17:48:27 by cjulienn          #+#    #+#             */
/*   Updated: 2022/06/03 15:46:30 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	handle_path_err(t_shell *shell, char **cmd_args)
{
	ft_putstr_fd(MALLOC_ERR, STDERR_FILENO);
	free_split(cmd_args);
	clean_child_process(shell);	
	exit(EXIT_FAILURE);
}

/* every time a CMD which is NOT a built-in is launched,
check the env in order to find the PATH variable,
then store every possible path in a **char and add a slash to every path
if NO PATH VARIABLE IS FOUND, then return NULL (will display a
bash-like error msg at execution (bash : command not found))*/

static void	handle_slashes_prbl(t_shell *shell, char **paths_ws)
{
	free_problem_str_arr(paths_ws, shell->i);
	handle_path_err(shell, paths_ws);
}

static void	add_slashes(t_shell *shell, char **paths_ws, char**paths)
{
	char	*tmp;

	while (paths[shell->i])
	{
		paths_ws[shell->i] = ft_strdup(paths[shell->i]);
		if (!paths_ws[shell->i])
			handle_slashes_prbl(shell, paths_ws);
		tmp = ft_strjoin(paths_ws[shell->i], "/");
		if (!tmp)
		{
			free(paths_ws[shell->i]);
			handle_slashes_prbl(shell, paths_ws);
		}
		free(paths_ws[shell->i]);
		paths_ws[shell->i] = tmp;
		if (!paths_ws[shell->i])
			handle_slashes_prbl(shell, paths_ws);
		shell->i++;
	}
	paths_ws[shell->i] = NULL;
}

static char	**paths_with_slash(t_shell *shell, char **paths)
{
	size_t	arr_len;
	char	**paths_ws;

	arr_len = 0;
	while (paths[arr_len])
		arr_len++;
	paths_ws = (char **)malloc(sizeof(char *) * (arr_len + 1));
	if (!paths_ws)
		handle_path_err(shell, paths);
	shell->i = 0;
	add_slashes(shell, paths_ws, paths);
	free_split(paths);
	paths = NULL;
	return (paths_ws);
}

char	**recup_paths(t_shell *shell, char **cmd_args)
{
	char	**paths;
	char	*path_str;

	shell->i = 0;
	path_str = NULL;
	while (shell->env[shell->i])
	{
		if (!ft_strncmp(shell->env[shell->i], "PATH=", 5))
		{
			path_str = shell->env[shell->i] + 5;
			break ;
		}
		shell->i++;
	}
	if (!path_str)
		return (NULL);
	paths = ft_split(path_str, ':');
	if (!paths)
		handle_path_err(shell, cmd_args);
	paths = paths_with_slash(shell, paths);
	return (paths);
}
