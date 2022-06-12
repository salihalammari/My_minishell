/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sghajdao <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 21:06:56 by sghajdao          #+#    #+#             */
/*   Updated: 2022/05/29 21:06:58 by sghajdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	tokenizer_find_char(char *string, char needle)
{
	int	i;

	i = 0;
	while (string[i])
	{
		if (string[i] == needle || (string[i] == D_QUOTE && string[i - 1] == ' ') || (string[i] == QUOTE && string[i - 1] == ' '))
			return (i);
		if (string[i] == '?')
			return (i + 1);
		i++;
	}
	return (i);
}

int	fixing_for_norminette(t_struct *mini, char c, char *aux, int nbr)
{
	if (mini->token.quote == 0 && (c == QUOTE || c == D_QUOTE))
		mini->token.quote = c;
	else
	{
		if (mini->token.quote == c)
			mini->token.quote = 0;
		else
		{
			aux[nbr] = c;
			nbr++;
		}
	}
	return (nbr);
}

void	clean_option(t_struct *mini, char *in, int i)
{
	char	*copy;

	copy = NULL;
	if (in[i] == '-' && in[i + 1] == 'n' && in[i + 2] == ' ')
	{
		while (in[i] == '-' && in[i + 1] == 'n' && in[i + 2] == ' ')
		{
			mini->has_flag = true;
			i += 3;
		}
		//copy = ft_strdup(&in[i]);
		//free(in);
		//in = ft_strdup(copy);
		in = ft_memmove(in, in + i, ft_strlen(in));
		free(copy);
	}
	else if (in[i] == '-' && in[i + 1] == 'n' && in[i + 2] == 'n')
	{
		++i;
		while (in[i] == 'n')
			i++;
		if (in[i] == ' ')
		{
			//copy = ft_strdup(&in[i]);
			//free(in);
			//in = ft_strdup(copy);
			//free(copy);
			in = ft_memmove(in, in + i, ft_strlen(in));
			mini->has_flag = true;
		}
	}
}

void	tokenizer_clean_quotes(t_struct *mini, char *in, int i, int c)
{
	char	*aux;

	mini->token.quote = 0;
	aux = ft_strtrim(in, " ");
	mini->has_flag = false;
	clean_option(mini, in, i);
	while (in[i] == ' ')
		i++;
	while (in[i])
	{
		c = fixing_for_norminette(mini, in[i], aux, c);
		i++;
	}
	aux[c] = '\0';
	free(mini->token.to_print);
	mini->token.to_print = aux;
}

void	tokenizer(t_struct *mini)
{
	t_token	*tk;

	tk = init_tk();
	mini->token.quote = 0;
	tk->end = ft_strdup("");
	if (mini->line)
	{
		echo_expander(mini);
		while ((int)ft_strlen(mini->line) > tk->i)
		{
			if (mini->token.quote == 0 && (mini->line[tk->i] == QUOTE))
				mini->token.quote = mini->line[tk->i];
			else
			{
				if (mini->token.quote == mini->line[tk->i])
					mini->token.quote = 0;
				if (mini->line[tk->i] == '~' && mini->token.quote == 0)
					get_home_sign(mini, tk);
				else if (mini->line[tk->i] == '$' && mini->token.quote == 0)
					get_dollar_sign(mini, tk);
			}
			tk->len++;
			tk->i++;
		}
		finish_tokenizer(mini, tk);
	}
}