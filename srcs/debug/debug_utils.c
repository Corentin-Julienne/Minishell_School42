/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 16:54:30 by cjulienn          #+#    #+#             */
/*   Updated: 2022/06/08 12:10:22 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/* contains utils to actually have feedback on minishell features
NOT TO INCLUDE IN THE REPO FOR EVAL !!!!! */

/* this func is used to check if the tokenisation process actually succeeded */

void	display_every_token(t_token *token)
{
	int			i;

	i = 0;
	while (token)
	{
		printf("token [%i] = [%s] | type = %d\n", i, token->item, token->type);
		token = token->next;
		i++;
	}
}

void	inspect_char_arr(char **arr)
{
	int			i;

	i = 0;
	while (arr && arr[i])
	{
		dprintf(STDERR_FILENO, "[%i] = [%s]\n", i, arr[i]);
		i++;
	}
}

// to be used in a PARENT process only !!!

void	inspect_exit_status(t_shell *shell)
{
	dprintf(STDERR_FILENO, "the current exit code is : %i\n", shell->exit_status);
}
