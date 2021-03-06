/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xle-boul <xle-boul@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 15:49:57 by cjulienn          #+#    #+#             */
/*   Updated: 2022/06/22 10:41:23 by xle-boul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/* should check wether token type is a CMD or not
return 1 if the case, 0 otherwise 
the 2 cases identified are :
1) the token is the first token or is located just after a pipe
2) the token is an ARG situated after the filename for a redirection
(or the heredoc DELIMITER) */

static int	is_cmd(t_token *token)
{
	int		res;

	res = 0;
	if (token->type == ARG)
	{
		if (!token->prev || (token->prev && token->prev->type == PIPE))
			res = 1;
		if (token->prev && token->prev->type == REDIR_ARG)
			res = 1;
	}
	if (res > 0)
		lowercase_cmds(token);
	return (res);
}

/* find thew type of the token (among | < < >> >>)
check also if the token is entirely surrounded
by simple or double quotes (with is_within_quotes func)
ARG can be flags, commandes, arguments for function or > >> < << */

static void	find_token_type(t_token *token)
{		
	while (token)
	{
		if (!ft_strncmp(token->item, "|", 1) && ft_strlen(token->item) == 1)
			token->type = PIPE;
		else if (!ft_strncmp(token->item, "<<", 2)
			&& ft_strlen(token->item) == 2)
			token->type = HERE_DOC;
		else if (!ft_strncmp(token->item, ">>", 2)
			&& ft_strlen(token->item) == 2)
			token->type = RO_APPEND;
		else if (!ft_strncmp(token->item, "<", 1)
			&& ft_strlen(token->item) == 1)
			token->type = REDIR_INPUT;
		else if (!ft_strncmp(token->item, ">", 1)
			&& ft_strlen(token->item) == 1)
			token->type = REDIR_OUTPUT;
		else if (token->prev && token->prev->type <= 5
			&& token->prev->type > 1)
			token->type = REDIR_ARG;
		else
			token->type = ARG;
		if (is_cmd(token) == 1)
			token->type = CMD;
		token = token->next;
	}
}

/* the parse_user_input func is converting the string user_input
into tokens. Tokens are words, quotted expressions separated
by space or | < << >> >, or | < << >> >.
every token is a part of a linked list containing the str,
the type of the token, if it is surrounded by closed quotes or not
*/

static t_token	*rtn_new_elem(t_shell *shell, t_token *token,
				char *ui_cpy, char *item)
{
	t_token		*new_elem;

	if (!item)
	{
		free(ui_cpy);
		free_case_err(shell, token);
	}
	new_elem = token_new(ft_strdup(item));
	if (!new_elem)
	{
		if (!item)
			free(item);
		free(ui_cpy);
		free_case_err(shell, token);
	}
	free(ui_cpy);
	ui_cpy = NULL;
	free(item);
	item = NULL;
	return (new_elem);
}

t_token	*parse_user_input(t_shell *shell)
{
	t_token		*token;
	t_token		*new_elem;
	char		*item;
	char		*ui_cpy;
	int			added_item_len;

	token = NULL;
	added_item_len = 0;
	while (42)
	{
		if (ft_strlen(shell->user_input) <= (size_t)added_item_len
			|| is_spaces_only(shell->user_input + added_item_len) == 1)
			break ;
		ui_cpy = ft_substr(shell->user_input, added_item_len,
				ft_strlen(shell->user_input) - shell->item_length);
		if (!ui_cpy)
			free_case_err(shell, token);
		item = isolate_item(ui_cpy, shell, token);
		added_item_len += shell->item_length;
		new_elem = rtn_new_elem(shell, token, ui_cpy, item);
		token_add_back(&token, new_elem);
	}
	find_token_type(token);
	expand_token(token, shell);
	return (token);
}
