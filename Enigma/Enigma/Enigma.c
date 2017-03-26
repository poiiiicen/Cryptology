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

char plugboard[26] = {'Z', 'N', 'V', 'D', 'E', 'F', 'T', 'Y', 'K', 'U', 'I', 'L', 'P', 'B', 'O', 'M', 'W', 'R', 'X',
	'G', 'J', 'C', 'Q', 'S', 'H', 'A'};

char ring_setting[3] = {'W', 'E', 'B'};

char message_key[3];

char* enigma_encrypt_decrypt(char* p);

void shift(char* key);

int main() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 26; j++) {
			rev_rotor_table[i][(int)(rotor_table[i][j] - 'A')] = (char)(j + 'A');
		}
	}
	char* p = "SWJYPKBDSVSYSJPYXKSDGHFRPZUDWYPBWJXOMZOPBYYKUKGDTJOEFUBAZDBDVXKWRAKOHGSKXVGYFGVAGLCQ";
	char* s;
	for (int i = 0; i < 26; i++) {
		for (int j = 0; j < 26; j++) {
			for (int k = 0; k < 26; k++) {
				message_key[0] = (char)('A' + i);
				message_key[1] = (char)('A' + j);
				message_key[2] = (char)('A' + k);
				s = enigma_encrypt_decrypt(p);
				for (int ii = 0, l = (int)strlen(p); ii < l; ii++) {
					if (s[ii] == 'H' && s[ii + 1] == 'I' && s[ii + 2] == 'T' && s[ii + 3] == 'L' && s[ii + 4] == 'E' &&
						s[ii + 5] == 'R') {
						printf("MessageKey=%c%c%c\n", message_key[0], message_key[1], message_key[2]);
						printf("PlainText=%s\n", s);
						break;
					}
				}
			}
		}
	}
	getchar();
	return 0;
}

void shift(char* key) {
	key[2] = (char)((key[2] - 'A' + 1) % 26 + 'A');
	if (key[2] == setp_char[rotor_num[2] - 1] || key[1] == (setp_char[rotor_num[1] - 1] - 'A' + 25) % 26 + 'A') {
		key[1] = (char)((key[1] - 'A' + 1) % 26 + 'A');
		if (key[1] == setp_char[rotor_num[1] - 1]) {
			key[0] = (char)((key[0] - 'A' + 1) % 26 + 'A');
		}
	}
}

char* enigma_encrypt_decrypt(char* p) {
	int l = (int)strlen(p);
	char* s = malloc(sizeof(char) * l);
	char key[3] = {message_key[0], message_key[1], message_key[2]};
	char tmp;
	for (int i = 0; i < l; i++) {
		shift(key);
		tmp = p[i];
		tmp = plugboard[tmp - 'A'];
		tmp = (char)((tmp - 'A' + key[2] - ring_setting[2] + 26) % 26 + 'A');
		tmp = rotor_table[rotor_num[2] - 1][tmp - 'A'];
		tmp = (char)((tmp - 'A' - key[2] + ring_setting[2] + 26) % 26 + 'A');
		tmp = (char)((tmp - 'A' + key[1] - ring_setting[1] + 26) % 26 + 'A');
		tmp = rotor_table[rotor_num[1] - 1][tmp - 'A'];
		tmp = (char)((tmp - 'A' - key[1] + ring_setting[1] + 26) % 26 + 'A');
		tmp = (char)((tmp - 'A' + key[0] - ring_setting[0] + 26) % 26 + 'A');
		tmp = rotor_table[rotor_num[0] - 1][tmp - 'A'];
		tmp = (char)((tmp - 'A' - key[0] + ring_setting[0] + 26) % 26 + 'A');
		tmp = reflector[tmp - 'A'];
		tmp = (char)((tmp - 'A' + key[0] - ring_setting[0] + 26) % 26 + 'A');
		tmp = rev_rotor_table[rotor_num[0] - 1][tmp - 'A'];
		tmp = (char)((tmp - 'A' - key[0] + ring_setting[0] + 26) % 26 + 'A');
		tmp = (char)((tmp - 'A' + key[1] - ring_setting[1] + 26) % 26 + 'A');
		tmp = rev_rotor_table[rotor_num[1] - 1][tmp - 'A'];
		tmp = (char)((tmp - 'A' - key[1] + ring_setting[1] + 26) % 26 + 'A');
		tmp = (char)((tmp - 'A' + key[2] - ring_setting[2] + 26) % 26 + 'A');
		tmp = rev_rotor_table[rotor_num[2] - 1][tmp - 'A'];
		tmp = (char)((tmp - 'A' - key[2] + ring_setting[2] + 26) % 26 + 'A');
		tmp = plugboard[tmp - 'A'];
		s[i] = tmp;
	}
	s[l] = 0;
	return s;
}
