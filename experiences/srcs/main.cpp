/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:11 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/02 17:37:48 by gule-bat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include <sstream>

int main(int argc, char **argv)
{
	if (argc != 3)
		return (std::cerr << "./ft_irc port password" << std::endl, 1);

	int x = 0;
	std::stringstream s(argv[1]);
	s >> x;
	try
	{
		Server s(x, "passwd");
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}