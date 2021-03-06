/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_redirs.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xle-boul <xle-boul@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 13:59:29 by cjulienn          #+#    #+#             */
/*   Updated: 2022/06/23 10:32:09 by xle-boul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/* this function check using access wether the path of
the target fd is valid or not,
display an err msg if path is not valid */

static void	file_opener(t_shell *shell, int type, char *path)
{	
	if (type == REDIR_INPUT)
	{
		if (access(path, F_OK) != 0)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(path, STDERR_FILENO);
			ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		}
		shell->fd_in = open(path, O_RDONLY);
	}
	else if (type == HERE_DOC)
		shell->fd_in = open(".heredoc_tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else if (type == REDIR_OUTPUT)
		shell->fd_out = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
	else
		shell->fd_out = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
}

/* when an input redir fail cause of a file is not actually existing,
we want to stop the process to run (not execute other cmds),
then return 1 as an exit code
if within CHILD process, just return exit code of 1
if in PARENT process, we want to go back to another loop of minishell */

static void	handle_no_existing_file(t_shell *shell, t_token *token, int process)
{
	if (process == PARENT)
		shell->exit_status = EXIT_FAILURE;
	else
	{
		token_clear(token);
		clean_child_process(shell);
		exit(EXIT_FAILURE);
	}
}

/* operate_redir should take the parsing parameter and perform
a SINGLE redir per call, based on the parameter */

static void	handle_redir_output(t_shell *shell)
{
	dup2(shell->fd_out, STDOUT_FILENO);
	close(shell->fd_out);
}

int	operate_redir(t_shell *shell, t_token *redir_tk,
	t_token *token, int process)
{
	int			type;
	char		*path;

	type = redir_tk->type;
	path = redir_tk->next->item;
	file_opener(shell, type, path);
	if (type == REDIR_INPUT || type == HERE_DOC)
	{
		if (type == REDIR_INPUT && shell->fd_in == -1)
		{
			handle_no_existing_file(shell, token, process);
			return (-1);
		}
		if (type == HERE_DOC)
			handle_here_doc(shell, path);
		else
		{
			dup2(shell->fd_in, STDIN_FILENO);
			close(shell->fd_in);
		}
	}
	else if (type == REDIR_OUTPUT || type == RO_APPEND)
		handle_redir_output(shell);
	return (0);
}
