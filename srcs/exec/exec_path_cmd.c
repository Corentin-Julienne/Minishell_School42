/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xle-boul <xle-boul@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/19 18:02:11 by cjulienn          #+#    #+#             */
/*   Updated: 2022/06/15 13:47:01 by xle-boul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/* this function check if the path is provided by the user
by checking if there is a '/' in the first arg (i.e. the cmd)
return 1 if it is the case, 0 otherwise */

static int	is_path_already_provided(const char *first_arg)
{
	char	*cutted_cmd;

	cutted_cmd = ft_strchr(first_arg, '/');
	if (cutted_cmd && cutted_cmd[0] == '/')
		return (1);
	return (0);
}

/* will check wether the user already wrote the all path (checking the 
presence or abscence of a slash)
if not the case, it will "glue" together the path and the cmd,
then returning it */

static char	*join_cmd_to_path(t_shell *shell, char **cmd_args, int i)
{
	char	*path;

	if (is_path_already_provided(cmd_args[0]) == 1)
		path = ft_strdup(cmd_args[0]);
	else
		path = ft_strjoin(shell->paths[i], cmd_args[0]);
	if (!path)
	{
		ft_putstr_fd(MALLOC_ERR_MSG, STDERR_FILENO);
		free_split(cmd_args);
		clean_child_process(shell);
		exit(EXIT_FAILURE);
	}
	return (path);
}

/* in case the cmd is not found (i.e. no path provided,
PATH ENV variable deleted, no "homemade programm"
or not found using the paths), return exit code 127 and clean to avoid leaks */

static void	case_cmd_not_found(t_shell *shell, char **cmd_args)
{
	display_cmd_not_found(cmd_args, shell->paths);
	free_split(cmd_args);
	clean_child_process(shell);
	exit(CMD_NOT_FOUND);
}

/* split the arg str, then recuperating path in PATH cmd
if no path provided, because the env variable path was suppressed,
display a cmd not found bash-like msg and exit with code 127
otherwise, use execve :
It will try to assess every path. When one valid path is found,
will execute execve. Execve will kill the process, resolve leaks and exec cmd
otherwise, if no valid path is found, we clean the child process
and exit with code 127 (AKA cmd not found) */

static void	exec_os_cmd(t_shell *shell, char **updated_env, char **cmd_args)
{
	char		*path_with_cmd;

	while (shell->paths[shell->i])
	{
		path_with_cmd = join_cmd_to_path(shell, cmd_args, shell->i);
		if (is_path_functionnal(path_with_cmd, shell, cmd_args) == 0)
			execve(path_with_cmd, cmd_args, updated_env);
		free(path_with_cmd);
		path_with_cmd = NULL;
		shell->i++;
	}
}

void	path_cmd_exec(t_shell *shell, char **cmd_args)
{
	char		**updated_env;

	updated_env = update_env(shell->env_list);
	if (!updated_env)
	{
		free(cmd_args);
		clean_child_process(shell);
		exit(EXIT_FAILURE);
	}
	if (is_path_functionnal(cmd_args[0], shell, cmd_args) == 0)
		execve(cmd_args[0], cmd_args, updated_env);
	shell->paths = recup_paths(shell, cmd_args);
	if (!shell->paths)
		case_cmd_not_found(shell, cmd_args);
	close(shell->std_fdin);
	close(shell->std_fdout);
	shell->i = 0;
	exec_os_cmd(shell, updated_env, cmd_args);
	free_split(updated_env);
	case_cmd_not_found(shell, cmd_args);
}
