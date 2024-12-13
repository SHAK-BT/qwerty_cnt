#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>

#ifndef VERSION
#define VERSION "0.0.1"
#endif

#define QWERTY_LETTERS 27
#define MAX_NEIGHBOURS 7
#define ARGS_NUM 3

// The longest word that can be found in the English dictionary
// contains 45 letters and names the disease silicosis:
// Pneumonoultramicroscopicsilicovolcanoconiosis.
#define MAX_WORD_LEN 46


static char keys_map[QWERTY_LETTERS][MAX_NEIGHBOURS] = {
	// first qwerty row:
	{'q', 'w', 'a'},
	{'w', 'q', 'a', 's', 'e'},
	{'e', 'w', 's', 'd', 'r'},
	{'r', 'e', 'd', 'f', 't'},
	{'t', 'r', 'f', 'g', 'y'},
	{'y', 't', 'g', 'h', 'u'},
	{'u', 'y', 'h', 'j', 'i'},
	{'i', 'u', 'j', 'k', 'o'},
	{'o', 'i', 'k', 'l', 'p'},
	{'p', 'o', 'l'},
	// second qwerty row:
	{'a', 'q', 'z', 'w', 's'},
	{'s', 'w', 'a', 'z', 'x', 'd', 'e'},
	{'d', 'e', 's', 'x', 'c', 'f', 'r'},
	{'f', 'r', 'd', 'c', 'v', 'g', 't'},
	{'g', 't', 'f', 'v', 'b', 'h', 'y'},
	{'h', 'y', 'g', 'b', 'n', 'j', 'u'},
	{'j', 'u', 'h', 'n', 'm', 'k', 'i'},
	{'k', 'i', 'j', 'm', 'l', 'o'},
	{'l', 'o', 'k', 'p'},
	// third qwerty row:
	{'z', 'a', 's', 'x'},
	{'x', 's', 'z', 'd', 'c'},
	{'c', 'd', 'x', 'f', 'v'},
	{'v', 'f', 'c', 'b', 'g'},
	{'b', 'g', 'v', 'n', 'h'},
	{'n', 'h', 'b', 'm', 'j'},
	{'m', 'j', 'n', 'k'}
};


static void help()
{
	puts("Counting and outputting the number of words from dictionary-file");
	puts("that can be typed on adjacent qwerty keyboard's keys." VERSION "\n");
	puts("Options:");
	puts("    -f arg    input file name");
	puts("    -h arg    display this help");
}

bool checkWord(char *word)
{
	// Let's check if this word falls into at least one of the groups of
	// neighboring buttons.
	for(int i = 0; i<QWERTY_LETTERS; i++ )
	{
		bool resKeyGroup = true;
		// Go through all the letters in the word except the repeated ones
		for (int j=0; j<strlen(word)-1; j++)
		{
			//skip the doubled letter check
			if(word[j] == word[j+1])
				j++;

			int k = 0;
			bool resLetter = false;
			// Check if all letters of the word are included in the group of
			// neighboring buttons, and form the result using 'OR'
			while(keys_map[i][k] && k<MAX_NEIGHBOURS)
			{
				if(keys_map[i][k] == word[j])
				{
					resLetter |= true;
					break; // found this letter in neighbouring letters group
				}
				else
				{
					resLetter |= false;
				}
				k++;
			}
			if(resLetter)
			{
				resKeyGroup &= true;
			}
			else
			{
				resKeyGroup &= false;
				break;	// stop if at least one letter form word not found
			}
		}
		if(resKeyGroup)
			return true;
	}
	return false;
}


int main(int argc, char *argv[])
{
	int opt;
	FILE *dictFile;
	char const *fileName = NULL;
	while ((opt = getopt(argc, argv, "f:h")) != -1) {
		switch (opt) {
			case 'f': fileName = optarg; break;
			case 'h': help(); return EXIT_SUCCESS;
			default: error(EXIT_FAILURE, 0, "Try %s -h for help.", argv[0]);
		}
	}

	if (argc < ARGS_NUM)
		error(EXIT_FAILURE, 0, "Not enough arguments. Try '-h' for help");

	if (fileName) {
		if((dictFile = fopen(fileName, "rt")) != NULL)
		{
			int wordCnt = 0;
			int lineCnt = 0;
			char line[MAX_WORD_LEN];
			while (fgets(line, sizeof(line), dictFile)) {
				if(checkWord(line))
				{
					printf("%s", line);
					wordCnt++;
				}
				lineCnt++;
			}
			printf("Result: %d words from %d - printable\n", wordCnt, lineCnt);
		}
		if (dictFile < 0)
			error(EXIT_FAILURE, errno, "Can not open dictionary file");
	}
}
