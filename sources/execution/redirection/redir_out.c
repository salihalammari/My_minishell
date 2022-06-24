/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_out.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slammari <slammari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 21:05:10 by sghajdao          #+#    #+#             */
/*   Updated: 2022/06/24 14:55:32 by slammari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	get_output_file(t_struct *mini, char *file, int flags)
{
	char	*copy;

	take_off_quotes(file);
	copy = ft_substr(file, 0, find_char(file, ' '));
	malloc_check_strdup(copy);
	free(file);
	file = ft_strdup(copy);
	malloc_check_strdup(file);
	free(copy);
	if (file[0] == '$')
	{
		copy = ft_strdup(file);
		malloc_check_strdup(copy);
		free(file);
		file = expander(mini, copy);
		free(copy);
	}
	copy = NULL;
	mini->out_fd = open(file, O_APPEND | flags, 0644);
	free (file);
}

static int	double_redir_out(t_struct *mini, char *file, int flags, int j)
{
	if (mini->commands[j + 1] == NULL)
		file = NULL;
	else
		file = ft_strtrim(mini->commands[j + 1] + 1, " ");
	if (file == NULL)
	{
		printf(ERROR_REDI);
		g_ret_number = 258;
		return (0);
	}
	get_output_file(mini, file, flags);
	mini->c++;
	return (1);
}

int	redirect_out(t_struct *mini, int j)
{
	int		flags;
	char	*file;

	flags = O_WRONLY | O_CREAT;
	file = NULL;
	j = mini->c;

	if (mini->commands[j] && mini->commands[j][0] == '>')
	{
		if (mini->commands[j] && mini->commands[j][1] == '>')
		{

			if (!double_redir_out(mini, file, flags, j))
				return (0);
		}
		else
			if (!simple_redir_out(mini, j, flags))
				return (0);
		mini->last_redir = 1;
		if (mini->split.n_comand == 1)
		{
			free(mini->line);
			mini->line = NULL;
		}
	}
	return (1);
}

int	simple_redir_out(t_struct *mini, int j, int flags)
{
	char	*assist;
	char	*file;

	assist = ft_strtrim(&mini->commands[j][1], " ");
	file = ft_substr(assist, 0, find_char(assist, ' '));
	if (!file[0])
	{
		printf(ERROR_REDI);
		g_ret_number = 258;
		return (0);
	}
	take_off_quotes(file);
	if (file[0] == '$')
	{
		if (!redir_expander(mini, &file))
		{
			free(assist);
			free(file);
			return (0);
		}
	}
	mini->out_fd = open(file, flags | O_TRUNC, 0644);
	free(assist);
	free(file);
	return (1);
}

int	find_char(char *string, char needle)
{
	int	i;

	i = 0;
	while (string[i])
	{
		if (string[i] == needle)
			return (i);
		i++;
	}
	return (i);
}
