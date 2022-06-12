/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sghajdao <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 21:04:56 by sghajdao          #+#    #+#             */
/*   Updated: 2022/05/29 21:04:57 by sghajdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	redirect_in(t_struct *mini, int j, char *aux)
{
	char	**file;
	char	*copy;
	char	**split;

	if (mini->commands[j][0] == '<')
	{
		file = NULL;
		if (mini->commands[j][1] == '<')
		{
			file = double_redir(mini, file, j);
			if (!file)
				return (0);
		}
		else
		{
			file = ft_split(&mini->commands[j][1], ' ');
			if (!file)
			{
				printf("malloc error\n");
				exit(1);
			}
			if  (!file[0])
			{
				printf(ERROR_REDI);
				g_ret_number = 258;
				return (0);
			}
			if (file[0][0] == D_QUOTE || file[0][0] == QUOTE)
				mini->redir_flag = 1;
			take_off_quotes(file[0]);
			if (file[0][0] == '$' && mini->redir_flag == 0)
			{
				copy = ft_strdup(file[0]);
				free(file[0]);
				file[0] = expander(mini, copy);
				free(copy);
			}
			if ((access(file[0], F_OK) == -1 || access(file[0], R_OK) == -1) && mini->error_name_file == NULL)
				mini->error_name_file = ft_strdup(file[0]);
			else
				mini->in_fd = open(file[0], O_RDONLY, 0777);
		}
		aux = ft_strtrim(mini->line, " ");
		if (mini->split.n_comand == 1 || (aux[0] == '|'
				&& ft_strlen(aux) == 1))
		{
			free(mini->line);
			mini->line = new_comman(1, file);
		}
		free(aux);
		mini->last_redir = 0;
		free_char_array(file);
		split = ft_split(mini->commands[j], ' ');
		if (!split)
		{
			printf("minishell: malloc error\n");
			exit(1);
		}
		if (((!split[1] && split[0][0] == '<' && split[0][1] == '<') || (split[0][2] == '\0' && split[0][1] == '<' && split[1] && !split[2]) || (!mini->commands[j + 1] && !mini->error_name_file)) && j < 1)
		{
			free_char_array(split);
			return (0);
		}
		free_char_array(split);
	}
	return (1);
}

char	**double_redir(t_struct *mini, char **file, int j)
{
	int old_stdin;

	file = ft_split(&mini->commands[j][2], ' ');
	if  (!file)
	{
		printf("malloc error\n");
		exit(1);
	}
	if (!file[0])
	{
		printf(ERROR_REDI);
		g_ret_number = 258;
		return (NULL);
	}
	if (file[0][0] == D_QUOTE || file[0][0] == QUOTE)
		mini->redir_flag = 1;
	take_off_quotes(file[0]);
	old_stdin = dup(STDIN_FILENO);
	read_until (mini, file[0]);
	mini->in_fd = dup(0);
	dup2(old_stdin, STDIN_FILENO);
	close(old_stdin);
	mini->is_append++;
	return (file);
}

void	read_until(t_struct *mini, char *end)
{
	char	*line;
	char	*copy;
	int		fd[2];

	line = ft_strdup("");
	pipe(fd);
	while (1)
	{
		free(line);
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strncmp(line, end, ft_strlen(end)) == 0 && line[ft_strlen(end)] == '\0')
			break ;
		if (line[0] == '$' && mini->redir_flag == 0)
		{
			copy = ft_strdup(line);
			free(line);
			line = find_env(mini, &copy[1]);
			free(copy);
		}
		ft_putendl_fd(line, fd[1]);
	}
	dup2(fd[0], 0);
	close(fd[1]);
	close(fd[0]);
	free(line);
}

char	*new_comman(int i, char **str)
{
	char	*aux;
	char	*copy;

	aux = ft_strdup("");
	while (str[i] != NULL)
	{
		if (ft_strlen(aux) > 0)
		{
			copy = ft_strdup(aux);
			free(aux);
			aux = ft_strjoin(copy, " ");
			free(copy);
		}
		copy = ft_strdup(aux);
		free(aux);
		aux = ft_strjoin(copy, str[i]);
		free(copy);
		i++;
	}
	return (aux);
}