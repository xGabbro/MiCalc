#include "tag_utils.h"

void get_first_block_key(uint8 _Dest[6], const uint8 _KnownKey, const uint8 _KnownKeyBlock, const uint8 _KnownKeyType)
{
	
}

void comestero_calculate_key(uint8 _Dest[6], const uint8 _Previous_KEY[6], const uint8 _Block, const char _Type)
{

	if (_Block == 0 && _Type == 'A') 
	{
		const uint8 default_key[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };

		for (int i = 0; i < 6; ++i) _Dest[i] = default_key[i];

		return;
	}

	const uint8 xor_table_A[16][6] = {
		[1] = {0x0E, 0x3F, 0x57, 0xAF, 0x05, 0x4B},
		[2] = {0x03, 0x85, 0x64, 0x9A, 0x45, 0xCE},
		[3] = {0x01, 0x52, 0xD0, 0x81, 0x9A, 0x49},
		[4] = {0x07, 0x8F, 0xA7, 0x6B, 0xF3, 0xA2},
		[5] = {0x01, 0x9A, 0x8E, 0xBC, 0xAB, 0x42},
		[6] = {0x03, 0x7B, 0x17, 0xC0, 0xC6, 0x83},
		[7] = {0x01, 0xAD, 0x83, 0xD5, 0xD3, 0x94},
		[8] = {0x0F, 0x13, 0x71, 0x49, 0x01, 0xEF},
		[9] = {0x01, 0xE5, 0x91, 0x79, 0x75, 0x9A},
		[10] = {0x03, 0x48, 0x2C, 0x13, 0xCC, 0xAD},
		[11] = {0x01, 0x3E, 0x38, 0x19, 0x89, 0x66},
		[12] = {0x07, 0x78, 0x7D, 0x00, 0x82, 0xE8},
		[13] = {0x01, 0x03, 0x0B, 0x2D, 0xC7, 0x91},
		[14] = {0x03, 0xE2, 0x5A, 0x8A, 0xC8, 0x2E},
		[15] = {0x01, 0x32, 0x86, 0xEB, 0x2B, 0xD2}
	};

	const uint8 xor_table_B[16][6] = {
		[0] = {0x0F, 0xC3, 0x7E, 0xD8, 0xFA, 0x5C},
		[1] = {0x01, 0xB2, 0x8E, 0xEC, 0xB4, 0x3B},
		[2] = {0x03, 0x80, 0x91, 0xFF, 0xC7, 0x8B},
		[3] = {0x01, 0x76, 0xAD, 0xC5, 0x43, 0x65},
		[4] = {0x07, 0x47, 0xBB, 0xB7, 0x9B, 0xBB},
		[5] = {0x01, 0xB8, 0x63, 0x13, 0xD9, 0xDC},
		[6] = {0x03, 0xD0, 0x8D, 0xB4, 0x14, 0x6D},
		[7] = {0x01, 0x1F, 0xEF, 0xCC, 0x46, 0xA2},
		[8] = {0x0F, 0x21, 0x9B, 0x74, 0x0F, 0x80},
		[9] = {0x01, 0xE6, 0xA5, 0xD4, 0xDF, 0x7C},
		[10] = {0x03, 0x85, 0x84, 0x60, 0xB7, 0xF8},
		[11] = {0x01, 0x5A, 0xA2, 0xD7, 0x4C, 0x4A},
		[12] = {0x07, 0x31, 0x65, 0x0F, 0x62, 0x18},
		[13] = {0x01, 0xFF, 0x19, 0xEC, 0x3F, 0x4E},
		[14] = {0x03, 0xCF, 0x7C, 0x1C, 0xA9, 0xE0},
		[15] = {0x01, 0xAE, 0x6E, 0x3A, 0x05, 0xD9}
	};

	const uint8 xor_table_switch[16][6] = {
		[1] = {0x10, 0x7B, 0x94, 0x22, 0x59, 0x24},
		[2] = {0x10, 0x7E, 0x61, 0x47, 0xDB, 0x61},
		[3] = {0x10, 0x5A, 0x1C, 0x03, 0x02, 0x4D},
		[4] = {0x10, 0x92, 0x00, 0xDF, 0x6A, 0x54},
		[5] = {0x10, 0xB0, 0xED, 0x70, 0x18, 0xCA},
		[6] = {0x10, 0x1B, 0x77, 0x04, 0xCA, 0x24},
		[7] = {0x10, 0xA9, 0x1B, 0x1D, 0x5F, 0x12},
		[8] = {0x10, 0x9B, 0xF1, 0x20, 0x51, 0x7D},
		[9] = {0x10, 0x98, 0xC5, 0x8D, 0xFB, 0x9B},
		[10] = {0x10, 0x55, 0x6D, 0xFE, 0x80, 0xCE},
		[11] = {0x10, 0x31, 0xF7, 0x30, 0x45, 0xE2},
		[12] = {0x10, 0x78, 0xEF, 0x3F, 0xA5, 0x12},
		[13] = {0x10, 0x84, 0xFD, 0xFE, 0x5D, 0xCD},
		[14] = {0x10, 0xA9, 0xDB, 0x68, 0x3C, 0x03},
		[15] = {0x10, 0x35, 0x33, 0xB9, 0x12, 0x08}
	};

	if (_Type == 'A')
	{

		for (int i = 0; i < 6; i++)
			_Dest[i] = xor_table_A[_Block][i] ^ _Previous_KEY[i];

	}
	else if (_Type == 'B')
	{

		for (int i = 0; i < 6; i++)
			_Dest[i] = xor_table_B[_Block][i] ^ _Previous_KEY[i];

	}
	else if (_Type == '0')
	{

		for (int i = 0; i < 6; i++)
			_Dest[i] = xor_table_switch[_Block][i] ^ _Previous_KEY[i];

	}

}

void mizip_calculate_key(uint8 _Dest[6], const uint8 _UID[4], const uint8 _Block, const char _Type)
{

	//Key of block 0 is already defined
	if (_Block == 0)
		return;

	const uint8 byte_rules_keyA[6] = {0, 1, 2, 3, 0, 1};

	const uint8 byte_rules_keyB[6] = {2, 3, 0, 1, 2, 3};

	const uint8 xor_table_keyA[5][6] = {
		[1] = {0x09, 0x12, 0x5A, 0x25, 0x89, 0xE5},
		[2] = {0xAB, 0x75, 0xC9, 0x37, 0x92, 0x2F},
		[3] = {0xE2, 0x72, 0x41, 0xAF, 0x2C, 0x09},
		[4] = {0x31, 0x7A, 0xB7, 0x2F, 0x44, 0x90}};

	const uint8 xor_table_keyB[5][6] = {
		[1] = {0xF1, 0x2C, 0x84, 0x53, 0xD8, 0x21},
		[2] = {0x73, 0xE7, 0x99, 0xFE, 0x32, 0x41},
		[3] = {0xAA, 0x4D, 0x13, 0x76, 0x56, 0xAE},
		[4] = {0xB0, 0x13, 0x27, 0x27, 0x2D, 0xFD}};

	for (int i = 0; i < 6; i++)
	{
		if (_Type == 'A')
			_Dest[i] = _UID[byte_rules_keyA[i]] ^ xor_table_keyA[_Block][i];
		else if (_Type == 'B')
			_Dest[i] = _UID[byte_rules_keyB[i]] ^ xor_table_keyB[_Block][i];
	}

}