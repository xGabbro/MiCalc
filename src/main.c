//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lib/cstring/cstring.h"
#include "tag_utils.c"

#define ERROR "[!] "

#define VERSION "2.0.0 BETA"

void comestero(void);
void mizip(void);
void dump_parser(void);

//Declaration of functions

void comestero_calculate_key(uint8_t _Dest[6], const uint8_t _Previous_KEY[6], const uint8_t _Block, const char _Type);
void mizip_calculate_key(uint8_t _Dest[6], const uint8_t _UID[4], const uint8_t _Block, const char _Type);

void uid_to_uint8_t(uint8_t _Dest[8], char _UID[9]);
uint8_t char_to_uint8_t(char _c);

void wait_input(void);

void sas();

char buffer[1024];

//Functions
int main(void)
{

	sas();

	while (1)
	{
		printf("------------------------------\n");
		printf("MICALC v." VERSION "\n");
		printf("------------------------------\n");
		printf("[1] MiZip keys calculator     \n");
		printf("[2] Comestero keys calculator \n");
		printf("[3] Dump parser               \n");
		printf("[9] Exit                      \n");
		printf("------------------------------\n");

		printf(" => ");
		scanf("%s", buffer);

		int choose = string_to_int(buffer);

		printf("\n\n");

		if (choose == 1) mizip();
		else if (choose == 2) comestero();
		else if (choose == 3) dump_parser();
		else if (choose == 9) break;	

		printf("\n\n\n\n");
	}

	return 0;
}

void mizip(void) 
{

	uint8_t KeyA[5][6] = {
		[0] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}};
	uint8_t KeyB[5][6] = {
		[0] = {0xB4, 0xC1, 0x32, 0x43, 0x9E, 0xEF}};

	printf("Enter the UID of MiZip: [HEX] ");
	scanf("%s", buffer);

	if (string_lenght(buffer) != 8) 
	{
		printf(ERROR "UID must be 8 characters lenght! \n");
		return;
	}

	uint8_t UID[4];
	if (!string_to_uint8_array(UID, buffer)) 
	{
		printf(ERROR "Invalid characters into UID! \n");
		return;
	}

	printf("\nMizip keys (UID: 0x");
	for (int i = 0; i < 4; i++)
		printf("%02X", UID[i]);
	printf("):\n");

	for (int i = 0; i < 5; i++)
	{

		mizip_calculate_key(KeyA[i], UID, i, 'A');
		mizip_calculate_key(KeyB[i], UID, i, 'B');

		printf("\t- Block %d:\n", i);

		printf("\t\t> Key A: 0x");
		for (int i2 = 0; i2 < 6; i2++)
			printf("%02X", KeyA[i][i2]);
		printf("\n");

		printf("\t\t> Key B: 0x");
		for (int i2 = 0; i2 < 6; i2++)
			printf("%02X", KeyB[i][i2]);
		printf("\n");
	}

}

void comestero(void)
{
	comestero_keys comestero_keys_A;
	comestero_keys comestero_keys_B;

	//Key operations
	printf("Enter the known key: [HEX] ");
	scanf("%s", buffer);

	if (string_lenght(buffer) != 12) 
	{
		printf(ERROR "Key must be 12 characters lenght!");
		return;
	}

	uint8_t known_key[6];
	string_to_uint8_array(known_key, buffer);

	//block operations
	printf("Enter the block of the known key: [HEX] ");
	scanf("%s", buffer);

	int known_key_block = string_to_int(buffer);

	if (known_key_block < 0 || known_key_block > 15) 
	{
		printf(ERROR "Please enter a valid block! \n");
		return;
	}
	//---

	//type operations
	printf("Enter the type of known key: [A/b] ");
	scanf("%s", buffer);

	char known_key_type = char_uppercase(buffer[0]); //know key type ius now uppercase

	if (known_key_type != 'A' && known_key_type != 'B') 
	{
		printf(ERROR "Please enter a valid type!\n");
		return;
	}
	//---

	if (known_key_type == 'A')
	{

		if (known_key_block == 0x0) {
			printf(ERROR "Invalid block for calculate keys! \n");
			return;
		}

		memory_copy(comestero_keys_A[known_key_block], known_key, sizeof(known_key));

		comestero_calculate_key(comestero_keys_B[known_key_block], comestero_keys_A[known_key_block], known_key_block, '0');
	}
	else if (known_key_type == 'B')
	{

		memory_copy(comestero_keys_B[known_key_block], known_key, sizeof(known_key));

		if (known_key_block == 0x0)
		{
			comestero_calculate_key(comestero_keys_B[0x1], comestero_keys_B[0x0], 0x1, 'B');
			known_key_block = 0x1;
		}

		comestero_calculate_key(comestero_keys_A[known_key_block], comestero_keys_B[known_key_block], known_key_block, '0');

	}

	int block = known_key_block + 1;
	while (block != known_key_block)
	{

		if (block == 16)
		{
			block = 0;
			continue;
		}

		if (block == 0)
			comestero_calculate_key(comestero_keys_B[block], comestero_keys_B[15], block, 'B');
		else
			comestero_calculate_key(comestero_keys_B[block], comestero_keys_B[block - 1], block, 'B');

		//a
		if (block == 1)
			comestero_calculate_key(comestero_keys_A[block], comestero_keys_A[15], block, 'A');
		else
			comestero_calculate_key(comestero_keys_A[block], comestero_keys_A[block - 1], block, 'A');

		block += 1;
	}

	printf("\n\n");

	printf("Comestero keys (Known key: 0x");
	for (int i = 0; i < 6; ++i) printf("%02X", known_key[i]);
	printf(", Block: %d, Key type: %c): \n", known_key_block, known_key_type);
	for (int block = 0; block < 16; ++block)
	{

		printf("\t- Block %X:\n", block);

		printf("\t\t> Key A: 0x");
		for (int byte = 0; byte < 6; ++byte)
		{
			printf("%02X", comestero_keys_A[block][byte]);
		}
		printf("\n");

		printf("\t\t> Key B: 0x");
		for (int byte = 0; byte < 6; ++byte)
		{
			printf("%02X", comestero_keys_B[block][byte]);
		}
		printf("\n");
	}

	FILE * keys_file;
	keys_file = fopen("comestero", "w");

	for(int a = 0; a < 16; ++a) 
	{
		
		fprintf(keys_file, "%c", '{');
		for (int b = 0; b < 6; ++b)
		{
			if (b < 5)
				fprintf(keys_file, "0x%02X,", comestero_keys_A[a][b]);
			else
				fprintf(keys_file, "0x%02X", comestero_keys_A[a][b]);
		}
		fprintf(keys_file, "%s", "}, ");

		fprintf(keys_file, "%c", '{');
		for (int b = 0; b < 6; ++b)
		{
			if (b < 5)
				fprintf(keys_file, "0x%02X,", comestero_keys_B[a][b]);
			else
				fprintf(keys_file, "0x%02X", comestero_keys_B[a][b]);
		}
		
		if (a != 15)
			fprintf(keys_file, "%s", "}, ");
		else
			fprintf(keys_file, "%s", "} ");
		

	}

	fclose(keys_file);

}

void dump_parser(void) {
	 return;
}

void sas() {

	const uint8_t keys1[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x78,0x3E,0x57,0x2C,0x2A,0x09}, {0x69,0xF7,0x4D,0xE2,0xC7,0x16}, {0x79,0x8C,0xD9,0xC0,0x9E,0x32}, 
								   {0x6A,0x72,0x29,0x78,0x82,0xD8}, {0x7A,0x0C,0x48,0x3F,0x59,0xB9}, {0x6B,0x20,0xF9,0xF9,0x18,0x91}, {0x7B,0x7A,0xE5,0xFA,0x1A,0xDC}, 
								   {0x6C,0xAF,0x5E,0x92,0xEB,0x33}, {0x7C,0x3D,0x5E,0x4D,0x81,0x67}, {0x6D,0x35,0xD0,0x2E,0x40,0x71}, {0x7D,0x85,0x3D,0x5E,0x58,0xBB}, 
								   {0x6E,0x4E,0xC7,0xEE,0x86,0xF2}, {0x7E,0x55,0xB0,0xEA,0x4C,0xD6}, {0x6F,0xE3,0x44,0x3B,0x55,0x66}, {0x7F,0x4A,0x5F,0x26,0x0A,0x74}, 
								   {0x60,0xF0,0x35,0x72,0x54,0x89}, {0x70,0x6B,0xC4,0x52,0x05,0xF4}, {0x61,0x15,0xA4,0x0B,0x21,0x13}, {0x71,0x8D,0x61,0x86,0xDA,0x88}, 
								   {0x62,0x5D,0x88,0x18,0xED,0xBE}, {0x72,0x08,0xE5,0xE6,0x6D,0x70}, {0x63,0x63,0xB0,0x01,0x64,0xD8}, {0x73,0x52,0x47,0x31,0x21,0x3A}, 
								   {0x64,0x1B,0xCD,0x01,0xE6,0x30}, {0x74,0x63,0x22,0x3E,0x43,0x22}, {0x65,0x18,0xC6,0x2C,0x21,0xA1}, {0x75,0x9C,0x3B,0xD2,0x7C,0x6C}, 
								   {0x66,0xFA,0x9C,0xA6,0xE9,0x8F}, {0x76,0x53,0x47,0xCE,0xD5,0x8C}, {0x67,0xC8,0x1A,0x4D,0xC2,0x5D}, {0x77,0xFD,0x29,0xF4,0xD0,0x55} };

	const uint8_t keys2[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x92,0x25,0x2B,0xCA,0xDB,0x80}, {0x83,0xEC,0x31,0x04,0x36,0x9F}, {0x93,0x97,0xA5,0x26,0x6F,0xBB}, 
								   {0x80,0x69,0x55,0x9E,0x73,0x51}, {0x90,0x17,0x34,0xD9,0xA8,0x30}, {0x81,0x3B,0x85,0x1F,0xE9,0x18}, {0x91,0x61,0x99,0x1C,0xEB,0x55}, 
								   {0x86,0xB4,0x22,0x74,0x1A,0xBA}, {0x96,0x26,0x22,0xAB,0x70,0xEE}, {0x87,0x2E,0xAC,0xC8,0xB1,0xF8}, {0x97,0x9E,0x41,0xB8,0xA9,0x32}, 
								   {0x84,0x55,0xBB,0x08,0x77,0x7B}, {0x94,0x4E,0xCC,0x0C,0xBD,0x5F}, {0x85,0xF8,0x38,0xDD,0xA4,0xEF}, {0x95,0x51,0x23,0xC0,0xFB,0xFD}, 
								   {0x8A,0xEB,0x49,0x94,0xA5,0x00}, {0x9A,0x70,0xB8,0xB4,0xF4,0x7D}, {0x8B,0x0E,0xD8,0xED,0xD0,0x9A}, {0x9B,0x96,0x1D,0x60,0x2B,0x01}, 
								   {0x88,0x46,0xF4,0xFE,0x1C,0x37}, {0x98,0x13,0x99,0x00,0x9C,0xF9}, {0x89,0x78,0xCC,0xE7,0x95,0x51}, {0x99,0x49,0x3B,0xD7,0xD0,0xB3}, 
								   {0x8E,0x00,0xB1,0xE7,0x17,0xB9}, {0x9E,0x78,0x5E,0xD8,0xB2,0xAB}, {0x8F,0x03,0xBA,0xCA,0xD0,0x28}, {0x9F,0x87,0x47,0x34,0x8D,0xE5}, 
								   {0x8C,0xE1,0xE0,0x40,0x18,0x06}, {0x9C,0x48,0x3B,0x28,0x24,0x05}, {0x8D,0xD3,0x66,0xAB,0x33,0xD4}, {0x9D,0xE6,0x55,0x12,0x21,0xDC}  };

	const uint8_t keys3[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x10,0x30,0x96,0x10,0x78,0x77}, {0x01,0xF9,0x8C,0xDE,0x95,0x68}, {0x11,0x82,0x18,0xFC,0xCC,0x4C}, 
								   {0x02,0x7C,0xE8,0x44,0xD0,0xA6}, {0x12,0x02,0x89,0x03,0x0B,0xC7}, {0x03,0x2E,0x38,0xC5,0x4A,0xEF}, {0x13,0x74,0x24,0xC6,0x48,0xA2}, 
								   {0x04,0xA1,0x9F,0xAE,0xB9,0x4D}, {0x14,0x33,0x9F,0x71,0xD3,0x19}, {0x05,0x3B,0x11,0x12,0x12,0x0F}, {0x15,0x8B,0xFC,0x62,0x0A,0xC5}, 
								   {0x06,0x40,0x06,0xD2,0xD4,0x8C}, {0x16,0x5B,0x71,0xD6,0x1E,0xA8}, {0x07,0xED,0x85,0x07,0x07,0x18}, {0x17,0x44,0x9E,0x1A,0x58,0x0A}, 
								   {0x08,0xFE,0xF4,0x4E,0x06,0xF7}, {0x18,0x65,0x05,0x6E,0x57,0x8A}, {0x09,0x1B,0x65,0x37,0x73,0x6D}, {0x19,0x83,0xA0,0xBA,0x88,0xF6}, 
								   {0x0A,0x53,0x49,0x24,0xBF,0xC0}, {0x1A,0x06,0x24,0xDA,0x3F,0x0E}, {0x0B,0x6D,0x71,0x3D,0x36,0xA6}, {0x1B,0x5C,0x86,0x0D,0x73,0x44}, 
								   {0x0C,0x15,0x0C,0x3D,0xB4,0x4E}, {0x1C,0x6D,0xE3,0x02,0x11,0x5C}, {0x0D,0x16,0x07,0x10,0x73,0xDF}, {0x1D,0x92,0xFA,0xEE,0x2E,0x12}, 
								   {0x0E,0xF4,0x5D,0x9A,0xBB,0xF1}, {0x1E,0x5D,0x86,0xF2,0x87,0xF2}, {0x0F,0xC6,0xDB,0x71,0x90,0x23}, {0x1F,0xF3,0xE8,0xC8,0x82,0x2B} };

	const uint8_t keys4[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x38,0xA3,0x9F,0xE3,0x9E,0xF5}, {0x29,0x6A,0x85,0x2D,0x73,0xEA}, {0x39,0x11,0x11,0x0F,0x2A,0xCE}, 
								   {0x2A,0xEF,0xE1,0xB7,0x36,0x24}, {0x3A,0x91,0x80,0xF0,0xED,0x45}, {0x2B,0xBD,0x31,0x36,0xAC,0x6D}, {0x3B,0xE7,0x2D,0x35,0xAE,0x20}, 
								   {0x2C,0x32,0x96,0x5D,0x5F,0xCF}, {0x3C,0xA0,0x96,0x82,0x35,0x9B}, {0x2D,0xA8,0x18,0xE1,0xF4,0x8D}, {0x3D,0x18,0xF5,0x91,0xEC,0x47}, 
								   {0x2E,0xD3,0x0F,0x21,0x32,0x0E}, {0x3E,0xC8,0x78,0x25,0xF8,0x2A}, {0x2F,0x7E,0x8C,0xF4,0xE1,0x9A}, {0x3F,0xD7,0x97,0xE9,0xBE,0x88}, 
								   {0x20,0x6D,0xFD,0xBD,0xE0,0x75}, {0x30,0xF6,0x0C,0x9D,0xB1,0x08}, {0x21,0x88,0x6C,0xC4,0x95,0xEF}, {0x31,0x10,0xA9,0x49,0x6E,0x74}, 
								   {0x22,0xC0,0x40,0xD7,0x59,0x42}, {0x32,0x95,0x2D,0x29,0xD9,0x8C}, {0x23,0xFE,0x78,0xCE,0xD0,0x24}, {0x33,0xCF,0x8F,0xFE,0x95,0xC6}, 
								   {0x24,0x86,0x05,0xCE,0x52,0xCC}, {0x34,0xFE,0xEA,0xF1,0xF7,0xDE}, {0x25,0x85,0x0E,0xE3,0x95,0x5D}, {0x35,0x01,0xF3,0x1D,0xC8,0x90}, 
								   {0x26,0x67,0x54,0x69,0x5D,0x73}, {0x36,0xCE,0x8F,0x01,0x61,0x70}, {0x27,0x55,0xD2,0x82,0x76,0xA1}, {0x37,0x60,0xE1,0x3B,0x64,0xA9} };

	const uint8_t keys7[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x39,0xBF,0x16,0xEA,0xCF,0x1A}, {0x28,0x76,0x0C,0x24,0x22,0x05}, {0x38,0x0D,0x98,0x06,0x7B,0x21}, 
								   {0x2B,0xF3,0x68,0xBE,0x67,0xCB}, {0x3B,0x8D,0x09,0xF9,0xBC,0xAA}, {0x2A,0xA1,0xB8,0x3F,0xFD,0x82}, {0x3A,0xFB,0xA4,0x3C,0xFF,0xCF}, 
								   {0x2D,0x2E,0x1F,0x54,0x0E,0x20}, {0x3D,0xBC,0x1F,0x8B,0x64,0x74}, {0x2C,0xB4,0x91,0xE8,0xA5,0x62}, {0x3C,0x04,0x7C,0x98,0xBD,0xA8}, 
								   {0x2F,0xCF,0x86,0x28,0x63,0xE1}, {0x3F,0xD4,0xF1,0x2C,0xA9,0xC5}, {0x2E,0x62,0x05,0xFD,0xB0,0x75}, {0x3E,0xCB,0x1E,0xE0,0xEF,0x67}, 
								   {0x21,0x71,0x74,0xB4,0xB1,0x9A}, {0x31,0xEA,0x85,0x94,0xE0,0xE7}, {0x20,0x94,0xE5,0xCD,0xC4,0x00}, {0x30,0x0C,0x20,0x40,0x3F,0x9B}, 
								   {0x23,0xDC,0xC9,0xDE,0x08,0xAD}, {0x33,0x89,0xA4,0x20,0x88,0x63}, {0x22,0xE2,0xF1,0xC7,0x81,0xCB}, {0x32,0xD3,0x06,0xF7,0xC4,0x29}, 
								   {0x25,0x9A,0x8C,0xC7,0x03,0x23}, {0x35,0xE2,0x63,0xF8,0xA6,0x31}, {0x24,0x99,0x87,0xEA,0xC4,0xB2}, {0x34,0x1D,0x7A,0x14,0x99,0x7F}, 
								   {0x27,0x7B,0xDD,0x60,0x0C,0x9C}, {0x37,0xD2,0x06,0x08,0x30,0x9F}, {0x26,0x49,0x5B,0x8B,0x27,0x4E}, {0x36,0x7C,0x68,0x32,0x35,0x46} };

	const uint8_t keys5[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x4D,0xD0,0xEF,0xF8,0x3B,0xEC}, {0x5C,0x19,0xF5,0x36,0xD6,0xF3}, {0x4C,0x62,0x61,0x14,0x8F,0xD7},
								   {0x5F,0x9C,0x91,0xAC,0x93,0x3D}, {0x4F,0xE2,0xF0,0xEB,0x48,0x5C}, {0x5E,0xCE,0x41,0x2D,0x09,0x74}, {0x4E,0x94,0x5D,0x2E,0x0B,0x39}, 
								   {0x59,0x41,0xE6,0x46,0xFA,0xD6}, {0x49,0xD3,0xE6,0x99,0x90,0x82}, {0x58,0xDB,0x68,0xFA,0x51,0x94}, {0x48,0x6B,0x85,0x8A,0x49,0x5E}, 
								   {0x5B,0xA0,0x7F,0x3A,0x97,0x17}, {0x4B,0xBB,0x08,0x3E,0x5D,0x33}, {0x5A,0x0D,0xFC,0xEF,0x44,0x83}, {0x4A,0xA4,0xE7,0xF2,0x1B,0x91}, 
								   {0x55,0x1E,0x8D,0xA6,0x45,0x6C}, {0x45,0x85,0x7C,0x86,0x14,0x11}, {0x54,0xFB,0x1C,0xDF,0x30,0xF6}, {0x44,0x63,0xD9,0x52,0xCB,0x6D}, 
								   {0x57,0xB3,0x30,0xCC,0xFC,0x5B}, {0x47,0xE6,0x5D,0x32,0x7C,0x95}, {0x56,0x8D,0x08,0xD5,0x75,0x3D}, {0x46,0xBC,0xFF,0xE5,0x30,0xDF}, 
								   {0x51,0xF5,0x75,0xD5,0xF7,0xD5}, {0x41,0x8D,0x9A,0xEA,0x52,0xC7}, {0x50,0xF6,0x7E,0xF8,0x30,0x44}, {0x40,0x72,0x83,0x06,0x6D,0x89}, 
								   {0x53,0x14,0x24,0x72,0xF8,0x6A}, {0x43,0xBD,0xFF,0x1A,0xC4,0x69}, {0x52,0x26,0xA2,0x99,0xD3,0xB8}, {0x42,0x13,0x91,0x20,0xC1,0xB0} };

	//Non si sa se sono corrette
	const uint8_t keys6[32][6] = { {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5}, {0x47,0xFD,0x55,0xED,0x3B,0x60}, {0x56,0x34,0x4F,0x23,0xD6,0x7F}, {0x46,0x4F,0xDB,0x01,0x8F,0x5B}, 
								   {0x55,0xB1,0x2B,0xB9,0x93,0xB1}, {0x45,0xCF,0x4A,0xFE,0x48,0xD0}, {0x54,0xE3,0xFB,0x38,0x09,0xF8}, {0x44,0xB9,0xE7,0x3B,0x0B,0xB5}, 
								   {0x53,0x6C,0x5C,0x53,0xFA,0x5A}, {0x43,0xFE,0x5C,0x8C,0x90,0x0E}, {0x52,0xF6,0xD2,0xEF,0x51,0x18}, {0x42,0x46,0x3F,0x9F,0x49,0xD2}, 
								   {0x51,0x8D,0xC5,0x2F,0x97,0x9B}, {0x41,0x96,0xB2,0x2B,0x5D,0xBF}, {0x50,0x20,0x46,0xFA,0x44,0x0F}, {0x40,0x89,0x5D,0xE7,0x1B,0x1D}, 
								   {0x5F,0x33,0x37,0xB3,0x45,0xE0}, {0x4F,0xA8,0xC6,0x93,0x14,0x9D}, {0x5E,0xD6,0xA6,0xCA,0x30,0x7A}, {0x4E,0x4E,0x63,0x47,0xCB,0xE1}, 
								   {0x5D,0x9E,0x8A,0xD9,0xFC,0xD7}, {0x4D,0xCB,0xE7,0x27,0x7C,0x19}, {0x5C,0xA0,0xB2,0xC0,0x75,0xB1}, {0x4C,0x91,0x45,0xF0,0x30,0x53}, 
								   {0x5B,0xD8,0xCF,0xC0,0xF7,0x59}, {0x4B,0xA0,0x20,0xFF,0x52,0x4B}, {0x5A,0xDB,0xC4,0xED,0x30,0xC8}, {0x4A,0x5F,0x39,0x13,0x6D,0x05}, 
								   {0x59,0x39,0x9E,0x67,0xF8,0xE6}, {0x49,0x90,0x45,0x0F,0xC4,0xE5}, {0x58,0x0B,0x18,0x8C,0xD3,0x34}, {0x48,0x3E,0x2B,0x35,0xC1,0x3C} };
	//----

	const uint8_t uid1[4] = {0x33, 0x33, 0xA3, 0xFE};
	const uint8_t uid2[4] = {0x66, 0xCF, 0x08, 0x24};
	const uint8_t uid3[4] = {0x66, 0x88, 0xAA, 0x24};
	const uint8_t uid4[4] = {0x63, 0x39, 0x95, 0x9F};
	const uint8_t uid7[4] = {0x63, 0x39, 0x95, 0x9E};
	const uint8_t uid5[4] = {0xB3, 0x4F, 0xAC, 0xFE};

	//Non si sa se sono corrette
	const uint8_t uid6[4] = {0x00, 0x00, 0x7F, 0x00};
	//----

	int a = 0;
	uint8_t temp;
	uint8_t xor;
	for (int i = 0; i < 16; ++i) 
	{
		xor = 0;

		printf("%i: ", i + 1);

		for (int i2 = 0; i2 < 4; ++i2)
		{

			if (i2 == 0)
				a =  (i >> i2) & 0b0001;
			else if (i2 == 1)
				a =  (i >> i2) & 0b001;
			else if (i2 == 2)
				a = (i >> i2) & 0b01;
			else if (i2 == 3)
				a = (i >> i2) & 0b1;

			if (a == 0b1)
				temp = ~uid5[i2];
			else
				temp = uid5[i2];

			xor ^= temp ^ 0b01110111;

			printf("%02X ", temp);

		}

		printf("- xor: %02X \n", xor);

	}

}