
// Extracted and slimmed down code of half_float from:
//  https://github.com/acgessler/half_float

///////////////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2006-2008, Alexander Gessler

All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the following 
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
///////////////////////////////////////////////////////////////////////////////////

#include "HalfFloat.hpp"

struct IEEE_16 {
	uint16_t Frac : 10;	// mantissa
	uint16_t Exp  : 5;		// exponent
	uint16_t Sign : 1;		// sign
};

struct IEEE_32 {
	uint32_t Frac : 23;
	uint32_t Exp  : 8;
	uint32_t Sign : 1;
};
union IEEESingle
{
	float Float;
	IEEE_32 IEEE;
};

float Float16ToFloat32(uint16_t half)
{
	union 
	{
		uint16_t bits;			// All bits
		IEEE_16 IEEE;
	};
	bits = half;
	
	IEEESingle sng;
	sng.IEEE.Sign = IEEE.Sign;

	if (!IEEE.Exp) 
	{  
		if (!IEEE.Frac)
		{
			sng.IEEE.Frac=0;
			sng.IEEE.Exp=0;
		}
		else
		{
			const float half_denorm = (1.0f/16384.0f); 
			float mantissa = ((float)(IEEE.Frac)) / 1024.0f;
			float sgn = (IEEE.Sign)? -1.0f :1.0f;
			sng.Float = sgn*mantissa*half_denorm;
		}
	}
	else if (31 == IEEE.Exp)
	{
		sng.IEEE.Exp = 0xff;
		sng.IEEE.Frac = (IEEE.Frac!=0) ? 1 : 0;
	}
	else 
	{
		sng.IEEE.Exp = IEEE.Exp+112;
		sng.IEEE.Frac = (IEEE.Frac << 13);
	}
	return sng.Float;
}

uint16_t Float32ToFloat16(float single)
{
	union 
	{
		uint16_t bits;			// All bits
		IEEE_16 IEEE;
	};
	
	IEEESingle f;
	f.Float = single;

	IEEE.Sign = f.IEEE.Sign;

	if ( !f.IEEE.Exp) 
	{ 
		IEEE.Frac = 0;
		IEEE.Exp = 0;
	}
	else if (f.IEEE.Exp==0xff) 
	{ 
		// NaN or INF
		IEEE.Frac = (f.IEEE.Frac!=0) ? 1 : 0;
		IEEE.Exp = 31;
	}
	else 
	{ 
		// regular number
		int new_exp = f.IEEE.Exp-127;

		if (new_exp<-24) 
		{ // this maps to 0
			IEEE.Frac = 0;
			IEEE.Exp = 0;
		}

		else if (new_exp<-14) 
		{
			// this maps to a denorm
			IEEE.Exp = 0;
			unsigned int exp_val = (unsigned int) (-14 - new_exp);  // 2^-exp_val
			switch (exp_val) 
			{
			case 0: 
				IEEE.Frac = 0; 
				break;
			case 1: IEEE.Frac = 512 + (f.IEEE.Frac>>14); break;
			case 2: IEEE.Frac = 256 + (f.IEEE.Frac>>15); break;
			case 3: IEEE.Frac = 128 + (f.IEEE.Frac>>16); break;
			case 4: IEEE.Frac = 64 + (f.IEEE.Frac>>17); break;
			case 5: IEEE.Frac = 32 + (f.IEEE.Frac>>18); break;
			case 6: IEEE.Frac = 16 + (f.IEEE.Frac>>19); break;
			case 7: IEEE.Frac = 8 + (f.IEEE.Frac>>20); break;
			case 8: IEEE.Frac = 4 + (f.IEEE.Frac>>21); break;
			case 9: IEEE.Frac = 2 + (f.IEEE.Frac>>22); break;
			case 10: IEEE.Frac = 1; break;
			}
		}
		else if (new_exp>15) 
		{ // map this value to infinity
			IEEE.Frac = 0;
			IEEE.Exp = 31;
		}
		else 
		{
			IEEE.Exp = new_exp+15;
			IEEE.Frac = (f.IEEE.Frac >> 13);
		}
	}
	return bits;
}

