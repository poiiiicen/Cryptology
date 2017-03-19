#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char rotor_table[5][27] =
{
	"EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	"AJDKSIRUXBLHWTMCQGZNPYFVOE",
	"BDFHJLCPRTXVZNYEIWGAKMUSQO",
	"ESOVPZJAYQUIRHXLNFTGKDCMWB",
	"VZBRGITYUPSDNHLXAWMJQOFECK"
};

char rev_rotor_table[5][27];
char reflector[] = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

char setp_char[5] = {'R', 'F', 'W', 'K', 'A'};

int rotor_num[3] = {5, 3, 2};

int plugboard[26] = {25, 13, 21, 3, 4, 5, 19, 24, 10, 20, 8, 11, 15, 1, 14, 12, 2, 17, 23, 6, 9, 2, 16, 18, 7, 0};

char ring_setting[3] = {'W', 'E', 'B'};

char message_key[3];

char* enigma_encrypt_decrypt(char* p);

void shift(char* key);

int main() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 26; j++) {
			rev_rotor_table[i][rotor_table[i][j] - 'A'] = j + 'A';
		}
	}
	char* p = "SWJYPKBDSVSYSJPYXKSDGHFRPZUDWYPBWJXOMZOPBYYKUKGDTJOEFUBAZDBDVXKWRAKOHGSKXVGYFGVAGLCQ";
	char* s;
	for (int i = 0; i < 26; i++) {
		for (int j = 0; j < 26; j++) {
			for (int k = 0; k < 26; k++) {
				message_key[0] = 'A' + i;
				message_key[1] = 'A' + j;
				message_key[2] = 'A' + k;
				s = enigma_encrypt_decrypt(p);
				for (int ii = 0, l = strlen(p); ii < l;) {
					if (s[ii] == 'H' && s[ii + 1] == 'I' && s[ii + 2] == 'T' && s[ii + 3] == 'L' && s[ii + 4] == 'E' && s[ii + 5] == 'R') {
						printf("%s", s);
						break;
					}
				}
			}

		}
	}
	return 0;
}

void shift(char* key) {
	key[2] = (key[2] - 'A' + 1) % 26 + 'A';
	if (key[2] == setp_char[rotor_num[2] - 1] || key[1] == (setp_char[rotor_num[1] - 1] - 'A' + 25) % 26 + 'A') {
		key[1] = (key[1] - 'A' + 1) % 26 + 'A';
		if (key[1] == setp_char[rotor_num[1] - 1]) {
			key[0] = (key[0] - 'A' + 1) % 26 + 'A';
		}
	}
}

char* enigma_encrypt_decrypt(char* p) {
	int l = strlen(p);
	char* s = malloc(sizeof(char) * l);
	char key[3] = {message_key[0], message_key[1], message_key[2]};
	//memcpy(s, p, l);
	for (int i = 0, tmp; i < l; i++) {
		shift(key);
		tmp = p[i] - 'A';
		tmp = plugboard[tmp];
		tmp = (tmp + key[2] - ring_setting[2] + 26) % 26;
		tmp = rotor_table[rotor_num[2] - 1][tmp] - 'A';
		tmp = (tmp - key[2] + ring_setting[2] + 26) % 26;
		tmp = (tmp + key[1] - ring_setting[1] + 26) % 26;
		tmp = rotor_table[rotor_num[1] - 1][tmp] - 'A';
		tmp = (tmp - key[1] + ring_setting[1] + 26) % 26;
		tmp = (tmp + key[0] - ring_setting[0] + 26) % 26;
		tmp = rotor_table[rotor_num[0] - 1][tmp] - 'A';
		tmp = (tmp - key[0] + ring_setting[0] + 26) % 26;
		tmp = reflector[tmp] - 'A';
		tmp = (tmp + key[0] - ring_setting[0] + 26) % 26;
		tmp = rev_rotor_table[rotor_num[0] - 1][tmp] - 'A';
		tmp = (tmp - key[0] + ring_setting[0] + 26) % 26;
		tmp = (tmp + key[1] - ring_setting[1] + 26) % 26;
		tmp = rev_rotor_table[rotor_num[1] - 1][tmp] - 'A';
		tmp = (tmp - key[1] + ring_setting[1] + 26) % 26;
		tmp = (tmp + key[2] - ring_setting[2] + 26) % 26;
		tmp = rev_rotor_table[rotor_num[2] - 1][tmp] - 'A';
		tmp = (tmp - key[2] + ring_setting[2] + 26) % 26;
		tmp = plugboard[tmp];
		s[i] = tmp + 'A';

		//tmp = (tmp + key[2] - ring_setting[2] + 26) % 26;
		//tmp = plugboard[tmp];
		//tmp = 
	}
	return s;
}
