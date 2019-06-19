/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_line.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: conoel <conoel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 13:49:22 by conoel            #+#    #+#             */
/*   Updated: 2019/06/19 18:42:56 by conoel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "asm.h"

static int	get_type(t_token_type type)
{
	int		i;

	i = 0;
	while (g_op_tab[i].type != type)
	{
		if (i == 17)
			break;
		i++;
	}
	return (i);
}

void		translate_indirect(t_token *param, int *len, char *buff)
{
	unsigned short	nb;

	if (param->content[0] == LABEL_CHAR)
		nb = 0;
	else
		nb = (short)ft_atoi(param->content);
	nb = reverse_bits_s(nb);
	buff[(*len)++] = nb & 0xFF;
	nb = nb >> 8;
	buff[(*len)++] = nb & 0xFF;
	return ;
}

void		translate_direct(t_token *param, int *len, char *buff)
{
	unsigned int	nb;
	int				i;
	char			*tmp;

	i = 0;
	if (param && param->content && param->content[1] == LABEL_CHAR)
	{
		tmp = param->content;
		param->content = ft_strdup(&(param->content[1]));
		free(tmp);
		translate_indirect(param, len, buff);
		return ;
	}
	nb = ft_atoi(&(param->content[2]));
	nb = reverse_bits(nb);
	while (i++ < 4)
	{
		buff[(*len)++] = nb & 0xFF;
		nb = nb >> 8;
	}	
	return ;
}

static int	param_encoding(t_token *param, int *len, char *buff)
{
	if (param->type == REG)
		buff[(*len)++] = param->content[1] - '0';
	if (param->type == DIRECT)  //TODO
		translate_direct(param, len, buff);
	if (param->type == INDIRECT)
		translate_indirect(param, len, buff);
	return (TRUE);
}

static char	ocp(t_token_type param, int i)
{
	int	ocp;

	ocp = 0;
	if (param == REG)
		ocp = 4;
	else if (param == DIRECT)
		ocp = 8;
	else if (param == INDIRECT)
		ocp = 12;
	while (i++ < 2)
		ocp = ocp * 4;
	return (ocp);
}

int			add_line(int fd, t_instruction *actual)
{
	char	*buff;
	int		len;
	int		i;
	int		type;

	if (!(buff = malloc(64)))
		return (FALSE);
	len = 1;
	type = get_type(actual->type);
	buff[0] = g_op_tab[type].opcode;
	if (g_op_tab[type].ocp)
		buff[len++] = 0;
	i = 0;
	while (i < actual->argc)
	{
		if (!(param_encoding(actual->args[i], &len, buff)))
			return (FALSE);
		if (g_op_tab[type].ocp)
			buff[1] += ocp(actual->args[i]->type, i);
		i++;
	}
	write(fd, buff, len);
	free(buff);
	return (TRUE);
}
