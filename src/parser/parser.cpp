/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbogad <jbogad@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 22:56:13 by jbogad            #+#    #+#             */
/*   Updated: 2026/02/25 23:20:35 by jbogad           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <string>
#include <iostream>


int main(int argc, char **av){

    char **a = av;
    
    if(argc != 3){
        std::cerr << "error -> a.out <port> <password>\n"; // nombre ejecutable etc etc+
        return 1;
    }
    std::cout << "todo bien de momento\n";
}
