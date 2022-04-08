//------------------------------------------------------------------------------
// pasm.cc: psim assembler 
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
#include <iostream>
#include <map>
#include <string>

#include "psim.h"

//------------------------------------------------------------------------------
// Global Constants
//------------------------------------------------------------------------------

static bool UnifiedMemory;

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------

int		main		    (int argc, char *argv[]) {
    LabelTable	lt;
    DataList	dl;
    TextList	tl;
    int		i;

    if (argc < 2) {
	std::cerr << "usage: pasm s0, s1, s2 ..." << std::endl;
	return (EXIT_FAILURE);
    }

    if (strncmp(argv[1], "u", 2) == 0) {
	UnifiedMemory = true;
	i = 2;
    } else {
	UnifiedMemory = false;
	i = 1;
    }

    for (; i < argc; i++) {
	std::ifstream	src;
	std::ofstream	tgt;
	std::string	tgt_file;

	src.open(argv[i]);

	if (src.is_open()) {
	    parse_stream(src, lt, dl, tl);

#ifdef __DEBUG__/*{{{*/
	    std::cout << "Label Table = " << std::endl;
	    for (LabelTable::iterator lti = lt.begin(); lti != lt.end(); lti++)
		std::cout << lti->first << " = " << lti->second << std::endl;
	    std::cout << std::endl;

	    std::cout << "Data List = " << std::endl;
	    for (size_t d = 0; d < dl.size(); d++)
		std::cout << dl[d] << std::endl;
	    std::cout << std::endl;
	    
	    std::cout << "Text List = " << std::endl;
	    for (size_t t = 0; t < tl.size(); t++) {
		for (size_t y = 0; y < tl[t].size(); y++) 
		    std::cout << tl[t][y] << " ";
		std::cout << std::endl;
	    }
	    std::cout << std::endl;
#endif/*}}}*/

	    tgt_file = argv[i];
	    tgt_file.erase(tgt_file.rfind("."));
	    tgt_file += (UnifiedMemory ? ".ubin" : ".bin");
	    tgt.open(tgt_file.c_str());
	    assemble_stream(tgt, lt, dl, tl);
	    tgt.close();
	} else {
	    std::cerr << "unable to open source file: " << argv[i] << std::endl;
	    return (EXIT_FAILURE);
	}

	lt.clear();
	dl.clear();
	tl.clear();

	src.close();
    }

    return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
// Assemble Stream
//------------------------------------------------------------------------------

bool		assemble_stream	    (std::ostream& out, LabelTable& lt, DataList& dl, TextList& tl) {
    Tokens  tokens;
    CWord   C;
    JWord   J;
    LWord   L;
    RWord   Ra;
    RWord   Rb;
    RWord   Rc;

    for (size_t i = 0; i < tl.size(); i++) {
	tokens = tl[i];
	
#if __DEBUG__/*{{{*/
	std::cout << "tokens[ " << i << "] =";
	for (size_t j = 0; j < tokens.size(); j++)
	    std::cout << " " << tokens[j];
	std::cout << std::endl;
#endif/*}}}*/

	if (tokens[0] == "ADD" || tokens[0] == "SUB") {
	    if (tokens.size() == 4 &&
		token_is_register(tokens[1]) &&
		token_is_register(tokens[2]) &&
		token_is_register(tokens[3])) {
		Ra = RWord(strtol(tokens[1].substr(1).c_str(), NULL, 10));
		Rb = RWord(strtol(tokens[2].substr(1).c_str(), NULL, 10));
		Rc = RWord(strtol(tokens[3].substr(1).c_str(), NULL, 10));
		out << (tokens[0] == "ADD" ? OWord(OP_ADD) : OWord(OP_SUB)) << Ra << Rb << Rc;
	    } else {
		std::cerr << "Invalid " << tokens[0] << " instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else if (tokens[0] == "MOV") {
	    if (tokens.size() == 3) {
		if (token_is_register(tokens[1])) {
		    Ra = RWord(strtol(tokens[1].substr(1).c_str(), NULL, 10));

		    if (token_is_constant(tokens[2])) {
			C = CWord(strtol(tokens[2].substr(1).c_str(), NULL, 10));
			out << OWord(OP_LOADC) << Ra << C;
		    } else if (token_is_address(tokens[2])) {
			if (get_label_value(lt, tokens[2].substr(1)) < 0)
			    goto AS_LABEL_ERROR;
			L = LWord(lt[tokens[2].substr(1)]);
			out << OWord(OP_LOADC) << Ra << L;
		    } else if (token_is_label(tokens[2])) {
			if (get_label_value(lt, tokens[2]) < 0)
			    goto AS_LABEL_ERROR;
			L = LWord(lt[tokens[2]]);
			out << OWord(OP_LOAD) << Ra << L; 
		    } else if (token_is_number(tokens[2])) {
			C = CWord(strtol(tokens[2].c_str(), NULL, 10));
			out << OWord(OP_LOAD) << Ra << C;
		    } else {
			goto AS_MOV_ERROR;
		    }
		} else {
		    Ra = RWord(strtol(tokens[2].substr(1).c_str(), NULL, 10));

		    if (token_is_label(tokens[1])) {
			if (get_label_value(lt, tokens[1]) < 0)
			    goto AS_LABEL_ERROR;
			L = LWord(lt[tokens[1]]);
			out << OWord(OP_STORE) << Ra << L;
		    } else if (token_is_number(tokens[1])) {
			C = CWord(strtol(tokens[1].c_str(), NULL, 10));
			out << OWord(OP_STORE) << Ra << C;
		    } else {
			goto AS_MOV_ERROR;
		    }
		}
	    } else if (tokens.size() == 4 &&
		       token_is_dio(tokens[1]) &&
		       token_is_register(tokens[2]) &&
		       token_is_pio(tokens[3])) {
		Ra = RWord(strtol(tokens[2].substr(1).c_str(), NULL, 10));
		Rb = RWord((strtol(tokens[3].substr(1).c_str(), NULL, 10) << 1) + 
			   (strtol(tokens[1].substr(1).c_str(), NULL, 10)));
		Rc = RWord(0);
		out << OWord(OP_IO) << Ra << Rb << Rc;
	    } else {
AS_MOV_ERROR:
		std::cerr << "Invalid MOV instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else if (tokens[0] == "MOVR") {
	    if (tokens.size() == 4 && token_is_register(tokens[1]) && token_is_register(tokens[2])) {
		Ra = RWord(strtol(tokens[1].substr(1).c_str(), NULL, 10));
		Rb = RWord(strtol(tokens[2].substr(1).c_str(), NULL, 10));
		if (token_is_constant(tokens[3])) {
		    Rc = RWord(strtol(tokens[3].substr(1).c_str(), NULL, 10));
		    out << OWord(OP_MOVR) << Ra << Rb << Rc;
		} else if (token_is_address(tokens[3])) {
		    if (get_label_value(lt, tokens[3].substr(1)) < 0)
			goto AS_LABEL_ERROR;
		    Rc = RWord(lt[tokens[3].substr(1)]);
		    out << OWord(OP_MOVR) << Ra << Rb << Rc;
		} else {
		    goto AS_MOVR_ERROR;
		}
	    } else {
AS_MOVR_ERROR:
		std::cerr << "Invalid MOVR instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else if (tokens[0] == "JMPZ") {
	    if (tokens.size() == 3 && token_is_register(tokens[1])) {
		Ra = RWord(strtol(tokens[1].substr(1).c_str(), NULL, 10));

		if (token_is_label(tokens[2])) {
		    if (get_label_value(lt, tokens[2]) < 0)
			goto AS_LABEL_ERROR;
		    L = LWord(lt[tokens[2]] - i);
		    out << OWord(OP_JMPZ) << Ra << L;
		} else if (token_is_number(tokens[2])) {
		    C = CWord(strtol(tokens[1].c_str(), NULL, 10));
		    out << OWord(OP_JMPZ) << Ra << C;
		} else {
		    goto AS_JMPZ_ERROR;
		}
	    } else {
AS_JMPZ_ERROR:
		std::cerr << "Invalid JMPZ instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else if (tokens[0] == "JMPN") {
	    if (tokens.size() == 3 && token_is_register(tokens[1])) {
		Ra = RWord(strtol(tokens[1].substr(1).c_str(), NULL, 10));

		if (token_is_label(tokens[2])) {
		    if (get_label_value(lt, tokens[2]) < 0)
			goto AS_LABEL_ERROR;
		    L = LWord(lt[tokens[2]] - i);
		    out << OWord(OP_JMPN) << Ra << L;
		} else if (token_is_number(tokens[2])) {
		    C = CWord(strtol(tokens[1].c_str(), NULL, 10));
		    out << OWord(OP_JMPN) << Ra << C;
		} else {
		    goto AS_JMPN_ERROR;
		}
	    } else {
AS_JMPN_ERROR:
		std::cerr << "Invalid JMPN instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else if (tokens[0] == "JMP") {
	    if (tokens.size() == 2) {
		if (token_is_label(tokens[1])) {
		    if (get_label_value(lt, tokens[1]) < 0)
			goto AS_LABEL_ERROR;
		    J = JWord(lt[tokens[1]] - i);
		} else if (token_is_number(tokens[1])) {
		    J = JWord(strtol(tokens[1].c_str(), NULL, 10));
		} else {
		    goto AS_JMP_ERROR;
		}
		out << OWord(OP_JMP) << J;
	    } else {
AS_JMP_ERROR:
		std::cerr << "Invalid JMP instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else if (tokens[0] == "END") {
	    if (tokens.size() == 1) {
		out << OWord(OP_END) << JWord(0);
	    } else {
		std::cerr << "Invalid END instruction (" << tokens_to_string(tokens) << ")" << std::endl;
		return (false);
	    }
	} else {
	    std::cerr << "Unknown instruction (" << tokens[0] << ")" << std::endl;
	    return (false);
	}

	out << std::endl;
    }

    if (UnifiedMemory)
	for (size_t i = 0; i < dl.size(); i++) 
	    out << dl[i] << std::endl;

    return (true);

AS_LABEL_ERROR:
    std::cerr << "Unknown label in instruction (" << tokens_to_string(tokens) << ")" << std::endl;
    return (false);
}

//------------------------------------------------------------------------------
// Parse Stream
//------------------------------------------------------------------------------

bool		parse_stream	    (std::istream& in, LabelTable& lt, DataList& dl, TextList& tl) {
    enum	ParseState  { ST_DATA, ST_TEXT };

    LabelTable	dt;
    std::string	line;
    std::string	label;
    size_t	data_addr;
    size_t	inst_addr;
    Tokens	tokens;
    ParseState	state;

    data_addr	= inst_addr = 0;
    state	= ST_TEXT;

    while (!in.eof()) {
	getline(in, line);
	trim_comment(line);

	if (line.size() == 0)	continue;

	label = get_label(line);

	trim_whitespace(label);
	trim_whitespace(line);

#ifdef __DEBUG__/*{{{*/
	std::cout << "[D] label = (" << label << ") line = (" << line << ")" << std::endl;
#endif/*}}}*/
	

	if (line.size())
	    tokens = tokenize(line);
	else
	    continue;

	for (size_t i = 0; i < tokens.size(); i++)
	    trim_comma(tokens[i]);

#ifdef __DEBUG__/*{{{*/
		std::cout << "tokens = ";
		for (size_t i = 0; i < tokens.size(); i++) {
		    std::cout << "(" << tokens[i] << ") ";
		}
		std::cout << std::endl;
#endif/*}}}*/

	if (line == ".data") {
	    state = ST_DATA;
	    continue;
	} else if (line == ".text") {
	    state = ST_TEXT;
	    continue;
	}

	switch (state) {
	    case ST_DATA:
		if (label.size() != 0) dt[label] = data_addr;

		if (tokens[0] == "WORD") {
		    for (size_t j = 1; j < tokens.size(); j++) {
			data_addr++;
			dl.push_back(DWord(strtol(tokens[j].c_str(), NULL, 10)));
		    }
		} else {
		    std::cerr << "Unknown data directive (" << tokens[0] << ")" << std::endl;
		    return (false);
		}
		break;
	    case ST_TEXT:
		if (label.size() != 0) lt[label] = inst_addr;
		if (tokens[0] == "MOV" ||
		    tokens[0] == "ADD" ||
		    tokens[0] == "SUB" ||
		    tokens[0] == "JMPZ" ||
		    tokens[0] == "JMPN" ||
		    tokens[0] == "JMP" ||
		    tokens[0] == "MOVR" ||
		    tokens[0] == "END") {
		    tl.push_back(tokens);
		} else {
		    std::cerr << "Unknown instruction (" << tokens[0] << ")" << std::endl;
		    return (false);
		}

		inst_addr++;
		break;
	}
    }

    for (LabelTable::iterator dti = dt.begin(); dti != dt.end(); dti++) 
	lt[dti->first] = (UnifiedMemory ? dti->second + tl.size() : dti->second);

    return (true);
}

//------------------------------------------------------------------------------
// vim: sts=4 sw=4 ts=8 ft=cpp
//------------------------------------------------------------------------------
