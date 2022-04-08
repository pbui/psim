//------------------------------------------------------------------------------
// psim_common.cc: psim common functions
//------------------------------------------------------------------------------

// Copyright (c) 2007 Peter Bui. All Rights Reserved.

// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.

// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:

// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.

// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.  

// 3. This notice may not be removed or altered from any source distribution.
//
// Peter Bui <pbui@cse.nd.edu>

//------------------------------------------------------------------------------

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "psim.h"

//------------------------------------------------------------------------------
// DWord to Long
//------------------------------------------------------------------------------

long		dword_to_long	    (DWord dw) {
    long    l;

    if (dw[dw.size() - 1] == 1) {
	dw = dw.flip();
	l = -(dw.to_ulong() + 1);
    } else {
	l = dw.to_ulong();
    }

    return (l);
}

//------------------------------------------------------------------------------
// DWord to Pretty String
//------------------------------------------------------------------------------

std::string	dword_to_string	    (DWord d) {
    std::string s;

    s = "";

    for (size_t i = 0; i < d.size(); i++)
	s.push_back(d[d.size() - 1 - i] + '0');

    return (s);
}

//------------------------------------------------------------------------------
// DWord to String
//------------------------------------------------------------------------------

std::string	dword_to_pretty_string	(DWord d) {
    std::stringstream ss;

    ss << std::setfill(' ') << std::setw(7) << dword_to_long(d);
    ss << " 0x" << std::hex << std::setfill('0') << std::setw(4) << d.to_ulong();
    for (size_t i = 0; i < WORD_SIZE/4; i++) 
	ss << " " << dword_to_string(d).substr(i*4, 4);

    return (ss.str());
}

//------------------------------------------------------------------------------
// Get Label
//------------------------------------------------------------------------------

std::string	get_label	    (std::string& s) {
    std::string l;
    size_t	i;

    i = s.find(":", 0);
    if (i != std::string::npos) {
	l = s.substr(0, i);
	s = s.substr(i + 1);
    } else {
	l = "";
    }

    return (l);
}

//------------------------------------------------------------------------------
// Get Label Value
//------------------------------------------------------------------------------

int		get_label_value	    (LabelTable& lt, std::string s) {
    return (lt.find(s) == lt.end() ? -1 : lt[s]);
}

//------------------------------------------------------------------------------
// LWord to Long
//------------------------------------------------------------------------------

long		lword_to_long	    (LWord lw) {
    long    l;

    if (lw[lw.size() - 1] == 1) {
	lw = lw.flip();
	l = -(lw.to_ulong() + 1);
    } else {
	l = lw.to_ulong();
    }

    return (l);
}

//------------------------------------------------------------------------------
// Tokenize
//------------------------------------------------------------------------------

Tokens		tokenize	    (std::string& s) {
    std::vector<std::string> t;
    size_t i, j;

    j = s.find_first_not_of(" \t,", 0);
    i = s.find_first_of(" \t,", j);

    while (i != std::string::npos || j != std::string::npos) {
	t.push_back(s.substr(j, i - j));
	j = s.find_first_not_of(" \t,", i);
	i = s.find_first_of(" \t,", j);
    }

    return (t);
}

//------------------------------------------------------------------------------
// Tokens is Address 
//------------------------------------------------------------------------------

bool		token_is_address    (std::string& s) {
    std::string ts;

    if (s.size() > 1 && s[0] == '@') {
	ts = s.substr(1);
	return (token_is_label(ts));
    }

    return (false);
}

//------------------------------------------------------------------------------
// Tokens is Constant
//------------------------------------------------------------------------------

bool		token_is_constant   (std::string& s) {
    if (s.size() > 1 && s[0] == '#') {
	for (size_t i = 1; i < s.size(); i++)
	    if (i == 1 && (s[i] == '-' || isdigit(s[i])))
		continue;
	    else if (!isdigit(s[i]))
		return (false);
	return (true);
    }

    return (false);
}

//------------------------------------------------------------------------------
// Tokens is D I/O
//------------------------------------------------------------------------------

bool		token_is_dio	    (std::string& s) {
    return (token_is_prefix_num(s, 'D'));
}

//------------------------------------------------------------------------------
// Tokens is Label
//------------------------------------------------------------------------------

bool		token_is_label	    (std::string& s) {
    if (s.size() > 0 && isalpha(s[0])) {
	for (size_t i = 1; i < s.size(); i++)
	    if (!isalnum(s[i]) && s[i] != '_')
		return (false);
	return (true);
    }

    return (false);
}

//------------------------------------------------------------------------------
// Tokens is Number
//------------------------------------------------------------------------------

bool		token_is_number	    (std::string& s) {
    if (s.size() > 0) {
	for (size_t i = 0; i < s.size(); i++)
	    if (i == 0 && (s[i] == '-' || isdigit(s[i])))
		continue;
	    else if (!isdigit(s[i]))
		return (false);
	return (true);
    }

    return (false);
}

//------------------------------------------------------------------------------
// Tokens is D I/O
//------------------------------------------------------------------------------

bool		token_is_pio	    (std::string& s) {
    return (token_is_prefix_num(s, 'P'));
}

//------------------------------------------------------------------------------
// Tokens is Prefix 
//------------------------------------------------------------------------------

bool		token_is_prefix_num (std::string& s, char c) {
    if (s.size() > 1 && s[0] == c) {
	for (size_t i = 1; i < s.size(); i++)
	    if (!isdigit(s[i]))
		return (false);
	return (true);
    }

    return (false);
}

//------------------------------------------------------------------------------
// Tokens is Register
//------------------------------------------------------------------------------

bool		token_is_register   (std::string& s) {
    return (token_is_prefix_num(s, 'R'));
}

//------------------------------------------------------------------------------
// Tokens to String
//------------------------------------------------------------------------------

std::string	tokens_to_string    (Tokens& tokens) {
    std::string	s;

    s = (tokens.size() ? tokens[0] : "");

    for (size_t i = 1; i < tokens.size(); i++)
	s += (" " + tokens[i]);

    return (s);
}

//------------------------------------------------------------------------------
// Trim Comma
//------------------------------------------------------------------------------

void		trim_comma	    (std::string& s) {
    size_t i;

    i = s.find(",", 0);
    if (i != std::string::npos) 
	s = s.erase(i); 

    return;
}

//------------------------------------------------------------------------------
// Trim Comment
//------------------------------------------------------------------------------

void		trim_comment	    (std::string& s) {
    size_t i;

    i = s.find("//", 0);
    if (i != std::string::npos) 
	s = s.erase(i); 

    return;
}

//------------------------------------------------------------------------------
// Trim Whitespace
//------------------------------------------------------------------------------

void		trim_whitespace	    (std::string &s) {
    size_t i;

    i = s.find_first_not_of(" \t\r\n");
    if (i != std::string::npos)
	s.erase(0, i);
    else
	s.clear();
    
    i = s.find_last_not_of(" \t\r\n");
    if (i != std::string::npos && (i != s.size()-1))
	s.erase(i + 1);
    return;
}

//------------------------------------------------------------------------------
// vim: sts=4 sw=4 ts=8 ft=cpp
//------------------------------------------------------------------------------
