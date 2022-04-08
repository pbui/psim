//------------------------------------------------------------------------------
// psim.h: psim
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

#ifndef	__PSIM_H__
#define	__PSIM_H__

#include <bitset>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

static const size_t WORD_SIZE	=   16;
static const size_t PRF_SIZE	=   8;
static const size_t RF_SIZE	=   16;

//------------------------------------------------------------------------------
// Type Definitions
//------------------------------------------------------------------------------

typedef std::bitset<4>			OWord;
typedef std::bitset<4>			RWord;
typedef std::bitset<8>			CWord;
typedef std::bitset<8>			LWord;
typedef std::bitset<12>			JWord;
typedef std::bitset<WORD_SIZE>		DWord;

typedef std::vector<DWord>		DataList;

typedef std::map<std::string, int>	LabelTable;

typedef std::vector<std::string>	Tokens;
typedef std::vector<Tokens>		TextList;

typedef std::vector<DWord>		Memory;
typedef std::vector<DWord>		RegisterFile;

//------------------------------------------------------------------------------
// Enumerations
//------------------------------------------------------------------------------

typedef enum {
    OP_LOAD	= 0,	// 0000
    OP_STORE,		// 0001
    OP_ADD,		// 0010
    OP_LOADC,		// 0011
    OP_SUB,		// 0100
    OP_JMPZ,		// 0101
    OP_JMPN,		// 0110
    OP_JMP,		// 0111
    OP_MOVR,		// 1000
    OP_IO	= 14,	// 1110
    OP_END	= 15,	// 1111
    OP_UNKNOWN
} OPCODE;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

extern bool	assemble_stream	    (std::ostream&, LabelTable&, DataList&, TextList&);
extern bool	parse_stream	    (std::istream&, LabelTable&, DataList&, TextList&);
extern std::string  get_label	    (std::string&);
extern int	get_label_value	    (LabelTable&, std::string);

extern Tokens	tokenize	    (std::string&);
extern bool	token_is_address    (std::string&);
extern bool	token_is_constant   (std::string&);
extern bool	token_is_dio	    (std::string&);
extern bool	token_is_label	    (std::string&);
extern bool	token_is_number	    (std::string&);
extern bool	token_is_pio	    (std::string&);
extern bool	token_is_prefix_num (std::string&, char);
extern bool	token_is_register   (std::string&);
extern std::string tokens_to_string (Tokens&);

extern void	trim_comma	    (std::string&);
extern void	trim_comment	    (std::string&);
extern void	trim_whitespace	    (std::string&);

extern bool	load_stream	    (std::istream&, Memory&, RegisterFile&, RegisterFile&);
extern long	dword_to_long	    (DWord);
extern std::string dword_to_pretty_string (DWord);
extern std::string dword_to_string  (DWord);
extern long	lword_to_long	    (LWord);
extern void	print_help	    ();
extern void	print_memory	    (Memory&, size_t, size_t);
extern void	print_pregfile	    (RegisterFile&);
extern void	print_regfile	    (RegisterFile&, size_t);
extern size_t	step		    (Memory&, RegisterFile&, RegisterFile&, size_t, size_t);

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
// vim: sts=4 sw=4 ts=8 ft=cpp
//------------------------------------------------------------------------------
