//------------------------------------------------------------------------------
// psim.cc: psim simulator 
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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "psim.h"

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------

int		main		    (int argc, char *argv[]) {
    Memory	    memory;
    RegisterFile    regfile(RF_SIZE);
    RegisterFile    pregfile(PRF_SIZE);
    Tokens	    tokens;
    std::string	    file;
    std::string	    line;
    size_t	    command;
    size_t	    index;
    size_t	    pc;
    DWord	    dw;

    command = 0;
    pc	    = 0;

    dw	    = 38;

    print_help();

    while (!std::cin.eof()) {
	std::cout << "[" << std::setfill('0') << std::setw(4) << command++ << "]-> ";

	getline(std::cin, line);
	tokens = tokenize(line);

	if (tokens[0] == "l" || tokens[0] == "load") {
	    std::ifstream src;

	    index = line.find(tokens[0]);
	    index = line.find_first_not_of(" \t", index + 1);
	    if (index == std::string::npos) {
		std::cerr << "Must specify source file" << std::endl;
		continue;
	    } else 
		file = line.substr(index);

	    src.open(file.c_str());
	    
	    if (!src.is_open() || !load_stream(src, memory, regfile, pregfile))
		std::cerr << "Unable to load assembly file: " << file << std::endl;

	    src.close();
	    
	    pc = 0;
	} else if (tokens[0] == "m" || tokens[0] == "printm") {
	    if (tokens.size() == 1) 
		print_memory(memory, 0, memory.size());
	    else if (tokens.size() == 2) 
		print_memory(memory, strtol(tokens[1].c_str(), NULL, 10), memory.size());
	    else if (tokens.size() == 3)
		print_memory(memory, strtol(tokens[1].c_str(), NULL, 10), strtol(tokens[2].c_str(), NULL, 10));
	    else
		std::cerr << "Invalid print command format: " << line << std::endl;
	} else if (tokens[0] == "o" || tokens[0] == "printo") {
	    print_pregfile(pregfile);
	} else if (tokens[0] == "r" || tokens[0] == "printr") {
	    print_regfile(regfile, pc);
	} else if (tokens[0] == "s" || tokens[0] == "step") {
	    if (tokens.size() == 1) {
		pc = step(memory, regfile, pregfile, pc, 1);
	    } else if (tokens.size() == 2) {
		pc = step(memory, regfile, pregfile, pc, strtol(tokens[1].c_str(), NULL, 10));
	    } else {
		std::cerr << "Invalid print command format: " << line << std::endl;
	    }
	} else if (tokens[0] == "p" || tokens[0] == "print") {
	    print_regfile(regfile, pc);
	    print_pregfile(pregfile);
	    print_memory(memory, 0, memory.size());
	} else if (tokens[0] == "i" || tokens[0] == "io") {
	    if (tokens.size() == 3 && token_is_number(tokens[1]) && token_is_number(tokens[2])) {
		pregfile[strtol(tokens[1].c_str(), NULL, 10)] = strtol(tokens[2].c_str(), NULL, 10);
	    } else {
		std::cerr << "Invalid io command format: " << line << std::endl;
	    }
	} else if (tokens[0] == "q" || tokens[0] == "quit") {
	    return (EXIT_SUCCESS);
	} else if (tokens[0] == "h" || tokens[0] == "help") {
	    print_help();
	} else {
	    std::cerr << "Invalid command: " << line << std::endl;
	}
    }

    return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
// Load Stream
//------------------------------------------------------------------------------

bool		load_stream	    (std::istream& in, Memory& m, RegisterFile& r, RegisterFile& p) {
    std::string	    word;

    m.clear();

    while (!in.eof()) {
	getline(in, word);

	if (word.size() == WORD_SIZE)
	    m.push_back(DWord(strtol(word.c_str(), NULL, 2))); 
    }

    for (size_t i = 0; i < r.size(); i++)   r[i] = 0;
    for (size_t i = 0; i < p.size(); i++)   p[i] = 0;

    return (true);
}

//------------------------------------------------------------------------------
// Print Help
//------------------------------------------------------------------------------

void		print_help	    () {
    std::cerr << "\tCommand   Description" << std::endl;
    std::cerr << "\t---------------------------------------------" << std::endl;
    std::cerr << "\tl <file>  Load binary file (must be unified memory)" << std::endl;
    std::cerr << "\ti <p> <v> Set pregister <p> to <v>" << std::endl;
    std::cerr << "\to         Print i/o pregister file" << std::endl;
    std::cerr << "\tp         Print register file, i/o, and memory" << std::endl;
    std::cerr << "\tm <s> <e> Print memory regions from s to e (s defaults to 0, e to end of memory)" << std::endl;
    std::cerr << "\tr         Print register file" << std::endl;
    std::cerr << "\ts <n>     Step n times (n defaults to 1)" << std::endl;
    std::cerr << "\tq         Quit this program" << std::endl;
    std::cerr << "\th         This help message" << std::endl;
}

//------------------------------------------------------------------------------
// Print Memory
//------------------------------------------------------------------------------

void		print_memory	    (Memory& m, size_t s, size_t e) {
    std::cout << "<MEM> Decimal Hex    Binary" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    for (; s <= e && s < m.size(); s++) 
	std::cout << "<" << std::setfill('0') << std::setw(3) << s << "> " << dword_to_pretty_string(m[s]) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

//------------------------------------------------------------------------------
// Print PRegister File 
//------------------------------------------------------------------------------

void		print_pregfile	    (RegisterFile& prf) {
    std::cout << "|REG| Decimal Hex    Binary" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    for (size_t p = 0; p < prf.size(); p++) 
	std::cout << "|P" << std::setfill('0') << std::setw(2) << p << "| " << dword_to_pretty_string(prf[p]) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

//------------------------------------------------------------------------------
// Print Register File 
//------------------------------------------------------------------------------

void		print_regfile	    (RegisterFile& rf, size_t pc) {
    std::cout << "|REG| Decimal Hex    Binary" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    for (size_t r = 0; r < rf.size(); r++) 
	std::cout << "|R" << std::setfill('0') << std::setw(2) << r << "| " << dword_to_pretty_string(rf[r]) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "[PC ] " << dword_to_pretty_string(DWord(pc)) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

//------------------------------------------------------------------------------
// Step
//------------------------------------------------------------------------------

size_t		step		    (Memory& m, RegisterFile& rf, RegisterFile& prf, size_t pc, size_t s) {
    DWord   inst;
    OWord   Op;
    RWord   Ra;
    RWord   Rb;
    RWord   Rc;
    LWord   L;

    long    ra;
    long    rb;
    long    rc;
    long    jl;

    for (size_t i = 0; i < s && pc < m.size(); i++) {
	inst = m[pc];

	Op = OWord(strtol(dword_to_string(inst).substr(0, 4).c_str(), NULL, 2));

	switch (Op.to_ulong()) {
	    case OP_LOAD:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		L  = LWord(strtol(dword_to_string(inst).substr(8, 8).c_str(), NULL, 2));

		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> MOV  R" << Ra.to_ulong() << ", " << L.to_ulong()
			    << std::endl;

		rf[Ra.to_ulong()] = m[L.to_ulong()];
		break;
	    case OP_STORE:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		L  = LWord(strtol(dword_to_string(inst).substr(8, 8).c_str(), NULL, 2));

		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> MOV  " << L.to_ulong() << ", R" << Ra.to_ulong()
			    << std::endl;
		
		m[L.to_ulong()] = rf[Ra.to_ulong()];
		break;
	    case OP_ADD:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		Rb = RWord(strtol(dword_to_string(inst).substr(8, 4).c_str(), NULL, 2));
		Rc = RWord(strtol(dword_to_string(inst).substr(12, 4).c_str(), NULL, 2));
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> ADD  R" << Ra.to_ulong()
			    << ", R" << Rb.to_ulong()
			    << ", R" << Rc.to_ulong()
			    << std::endl;

		rb = dword_to_long(rf[Rb.to_ulong()]);
		rc = dword_to_long(rf[Rc.to_ulong()]);

		rf[Ra.to_ulong()] = rb + rc;
		break;
	    case OP_LOADC:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		L  = LWord(strtol(dword_to_string(inst).substr(8, 8).c_str(), NULL, 2));

		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> MOV  R" << Ra.to_ulong() << ", #" << lword_to_long(L)
			    << std::endl;

		rf[Ra.to_ulong()] = L.to_ulong();
		if (L[L.size() - 1] == 1) 
		    rf[Ra.to_ulong()] |= DWord(65280); // 1111 1111 0000 0000
		break;
	    case OP_SUB:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		Rb = RWord(strtol(dword_to_string(inst).substr(8, 4).c_str(), NULL, 2));
		Rc = RWord(strtol(dword_to_string(inst).substr(12, 4).c_str(), NULL, 2));
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> SUB  R" << Ra.to_ulong()
			    << ", R" << Rb.to_ulong()
			    << ", R" << Rc.to_ulong()
			    << std::endl;

		rb = dword_to_long(rf[Rb.to_ulong()]);
		rc = dword_to_long(rf[Rc.to_ulong()]);
		
		rf[Ra.to_ulong()] = rb - rc;
		break;
	    case OP_JMPZ:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		L  = LWord(strtol(dword_to_string(inst).substr(8, 8).c_str(), NULL, 2));
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> JMPZ R" << Ra.to_ulong()
			    << ", " << lword_to_long(L)
			    << std::endl;
		
		ra = dword_to_long(rf[Ra.to_ulong()]);
		jl = lword_to_long(L);
		
		if (ra == 0) pc = pc + jl - 1;
		break;
	    case OP_JMPN:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		L  = LWord(strtol(dword_to_string(inst).substr(8, 8).c_str(), NULL, 2));
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> JMPN R" << Ra.to_ulong()
			    << ", " << lword_to_long(L)
			    << std::endl;

		ra = dword_to_long(rf[Ra.to_ulong()]);
		jl = lword_to_long(L);
		
		if (ra < 0) pc = pc + jl - 1;
		break;
	    case OP_JMP:
		L  = LWord(strtol(dword_to_string(inst).substr(8, 8).c_str(), NULL, 2));
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> JMP  " << lword_to_long(L)
			    << std::endl;

		jl = lword_to_long(L);
		
		pc = pc + jl - 1;
		break;
	    case OP_MOVR:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		Rb = RWord(strtol(dword_to_string(inst).substr(8, 4).c_str(), NULL, 2));
		L  = LWord(strtol(dword_to_string(inst).substr(12, 4).c_str(), NULL, 2));
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> MOVR R" << Ra.to_ulong() 
			    << ", R" << Rb.to_ulong() 
			    << ", #" << lword_to_long(L) 
			    << std::endl;

		jl = lword_to_long(L);

		rf[Ra.to_ulong()] = m[rf[Rb.to_ulong()].to_ulong() + jl];
		break;
	    case OP_IO:
		Ra = RWord(strtol(dword_to_string(inst).substr(4, 4).c_str(), NULL, 2));
		Rb = RWord(strtol(dword_to_string(inst).substr(8, 3).c_str(), NULL, 2));
		rc = strtol(dword_to_string(inst).substr(11, 1).c_str(), NULL, 2);
		
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> MOV  D" << rc
			    << ", R" << Ra.to_ulong() 
			    << ", P" << Rb.to_ulong()
			    << std::endl;

		if (rc) 
		    prf[Rb.to_ulong()] = rf[Ra.to_ulong()];
		else 
		    rf[Ra.to_ulong()] = prf[Rb.to_ulong()];
		break;
	    case OP_END:
		std::cout   << "[PC = " << std::setfill('0') << std::setw(6) << pc << "] "
			    << "Inst = " << dword_to_pretty_string(inst)
			    << " -> END"
			    << std::endl;
		return (pc);
		break;
	    default:
		std::cerr   << "Unknown opcode: " << Op << " in " << dword_to_pretty_string(inst) << std::endl;
		break;
	}

	pc++;
    }

    return (pc);
}

//------------------------------------------------------------------------------
// vim: sts=4 sw=4 ts=8 ft=cpp
//------------------------------------------------------------------------------
