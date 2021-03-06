/*
	There are three threads, Sifter, Hill and Replacement
	that repeatedly take input, encoded messages, and 
	output decoded messages depending on the type of
	encoding. Sifter reads and decided. 
*/
/*
	@author Daniel Swain Jr
*/

#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <map>
#include <Windows.h>

using namespace std;

#define END "end"
#define ASTERISK '*'
#define MAX 214748364

void* sifter(void*);
void* replacement(void*);
void* hillCase1(void*);
void* hillCase2(void*);

std::map<char, unsigned int> alphaToNumMap = {	/* Contains the alphabet mapped to integers 0 to 25. */
		{ 'A', 0 }, { 'B', 1 }, { 'C', 2 }, { 'D', 3 }, { 'E', 4 },
		{ 'F', 5 }, { 'G', 6 }, { 'H', 7 }, { 'I', 8 }, { 'J', 9 },
		{ 'K', 10 }, { 'L', 11 }, { 'M', 12 }, { 'N', 13 }, { 'O', 14 },
		{ 'P', 15 }, { 'Q', 16 }, { 'R', 17 }, { 'S', 18 }, { 'T', 19 },
		{ 'U', 20 }, { 'V', 21 }, { 'W', 22 }, { 'X', 23 }, { 'Y', 24 },
		{ 'Z', 25 }
};

std::map<unsigned int, char> numToAlphaMap = {	/* Like the above but vice versa */
		{ 0, 'A' }, { 1, 'B' }, { 2, 'C' }, { 3, 'D' }, { 4, 'E' },
		{ 5, 'F' }, { 6, 'G' }, { 7, 'H' }, { 8, 'I' }, { 9, 'J' },
		{ 10, 'K' }, { 11, 'L' }, { 12, 'M' }, { 13, 'N' }, { 14, 'O' },
		{ 15, 'P' }, { 16, 'Q' }, { 17, 'R' }, { 18, 'S' }, { 19, 'T' },
		{ 20, 'U' }, { 21, 'V' }, { 22, 'W' }, { 23, 'X' }, { 24, 'Y' },
		{ 25, 'Z' }
};

unsigned int errCounter = 0;	/* Count invalid codes to a max of 3. */

/*==========================================================================*/
int main()
{
	pthread_t sif;

	// Initialize Sifter thread
	pthread_create(&sif, NULL, sifter, NULL);

	// Wait for Sifter to finish
	pthread_join(sif, NULL);

	return 0;
}

/*==========================================================================*/
void* sifter(void* args) 
{
	vector<string> buffer; /* Hold the message(s). */
	string input = "";
	pthread_t rep, hil1, hil2;

	std::cout << "Enter codes, when done enter 'end': \n";

	while (errCounter < 3)	/* Until the user enters 'end' */
	{
		getline (cin, input); // TODO: Currently will give an error if return is pressed with no input.
		string code = "" + input;
		
		if (code.front() == 'e' && code.at(code.size()-1) == 'd' && code.size() == 3) {
			cout << "Goodbye." << endl;
			break;
		}

		for (unsigned int i = 0; i < code.length(); i++) /* Look for the asterisks add to vector. */
		{
			if (code.at(i) == ASTERISK)
			{
				buffer.push_back(code.substr(0, i)); /* Add the code */ 
				// Clear the code from the string. Skip the asterisk.
				code = code.substr(i + 1, code.length());
				i = 0; // Reset
			}
		}

		buffer.push_back(code);	 /*	Add everything else. */

		while (!buffer.empty()) 
		{
			string message = buffer.front(); /* First message in the buffer. */
			char algID = message.at(0);	 /* Decides which algorithm to do. */
			
			for (unsigned int i = 1; i < message.length(); i++) 
			{
				if (algID == ' ' && isdigit(message.at(i))) // If it grabbed a space, continue till a number. 
				{ 
					algID = message.at(i);
					message = message.substr(i, message.length()); //algID[0], message[1 -> message.length()-1]
					break;
				}
			}

			if (!isdigit(algID))
			{
				cout << endl << "No Algorithm ID found in code: " << message << endl;
				cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
			}
			else
			if (algID >= '1' && algID <= '3') /* Validate form */
			{
				if (algID == '1')	/* Do Replacement */
				{
					// Initialize Replacement thread
					pthread_create(&rep, NULL, replacement, &message);

					// Wait for thread to finish
					pthread_join(rep, NULL);
				}
				if (algID == '2')	/* Do Hill Case 1 */
				{
					// Initialize Hill Case 1 thread
					pthread_create(&hil1, NULL, hillCase1, &message);

					// Wait for thread to finish
					pthread_join(hil1, NULL);
				}
				if (algID == '3')	/* Do Hill Case 2 */
				{
					// Initialize Hill Case 2 thread
					pthread_create(&hil2, NULL, hillCase2, &message);

					// Wait for thread to finish
					pthread_join(hil2, NULL);
				}
			}
			else /* Error, increase error counter. */
			{
				cout << endl << "Algorithm ID number does not exist for code: " << message << endl;
				cout << "Number of errors left:  " << 3-(++errCounter) << endl;
			}

			buffer.erase(buffer.begin()); /* Remove the head. */
		}
	}

	pthread_exit(0);
	return (NULL);
}

/*==========================================================================*/
void* replacement(void* args)
{
	string message = *reinterpret_cast<std::string*>(args), decodedMessage = "";
	char decipherChar = message.at(1), originalChar, newChar;

	if (isdigit(decipherChar)) /* Check if the decipher char is a digit. */
	{ 
		cout << endl << "Error: Decipher character is a digit in Replacement Algorithm code. " << endl;
		cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
		pthread_exit(0);
		return (NULL);
	}

	if (decipherChar == ' ') 
	{
		for (unsigned int i = 2; i < message.length(); i++)
		{
			if (isalpha(message.at(i))) 
			{
				decipherChar = message.at(i);
				message = message.substr(i - 1, message.length());
				break;
			}
		}
	}

	int shift = alphaToNumMap[toupper(decipherChar)];
	shift = (shift % 10) + 2;

	for (unsigned int i = 2; i < message.size(); i++) {	
		originalChar = message.at(i);

		if (isdigit(originalChar)) {
			cout << endl << "Error: Digit found in Replacement Algorithm code." << endl;
			cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
			pthread_exit(0);
			return (NULL);
		}

		if (originalChar != ' ') {
			int check = alphaToNumMap[toupper(originalChar)] - shift;	

			if (check < 0) { /* Check for word wrap. */
				check = 26 + check;
			}

			newChar = numToAlphaMap[check]; /* Get new character. */
			decodedMessage += newChar; /* Add to decoded string. */
		}
		else {
			decodedMessage += ' ';
		}
	}
	cout << endl << "Replcament Algorithm: " << decodedMessage << endl;
	Sleep(100);
	pthread_exit(0);
	return (NULL);
}

/*==========================================================================*/
void* hillCase1(void* args) 
{
	string message = *reinterpret_cast<std::string*>(args),
		section_1, section_2, decodedMessage = "";

	int indexToSplit = 0;
	for (unsigned int i = 1; i < message.length(); i++) {
		if (isdigit(message[i])) {
			indexToSplit = i - 1;
			break;
		}
	}

	// Create substrings
	section_1 = message.substr(1, indexToSplit);
	section_2 = message.substr(indexToSplit, message.length() - 1);

	// Check if matrix has characters
	for (int i = 0; i < section_2.length(); i++) 
	{
		if (isalpha(section_2.at(i))) {
			cout << endl << "Error: Characters found in Hill Case 1 Algorithm matrix." << endl;
			cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
			pthread_exit(0);
			return (NULL);
		}
	}

	// Trim white space
	stringstream sstream(section_2);
	int a, b, c, d;
	a = b = c = d = MAX; /* Check if all the integers are filled. */
	int error = MAX; /* Error Integer to check if there are too many numbers. */
	sstream >> a >> b >> c >> d >> error;

	// Check if there are more than four integers
	if (error != MAX) 
	{
		cout << endl << "Error: Too many integers found in Hill Case 1 Algorithm." << endl;
		cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
		pthread_exit(0);
		return (NULL);
	}
	// Check if each entry in the 2x2 matrix has values.
	if (a == MAX || b == MAX || c == MAX || d == MAX)
	{
		cout << endl << "Error: All entries of 2x2 matrix are not filled in Hill Case 1 Algorithm." << endl;
		cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
		pthread_exit(0);
		return (NULL);
	}

	// Turn the sections into arrays
	char first, second;
	for (unsigned int i = 0; i < section_1.length();) {
		first = section_1[i];		// Get the first and second characters
		second = section_1[i + 1];

		if (isalpha(first) && isalpha(second)) {
			int x = alphaToNumMap[toupper(first)];	// Get the numeric value
			int y = alphaToNumMap[toupper(second)];
			// matrix multiplication then find alpha value
			decodedMessage += numToAlphaMap[((a * x) + (b * y)) % 26];
			decodedMessage += numToAlphaMap[((c * x) + (d * y)) % 26];

			i += 2;	// Go to next pair of characters
		}
		else {
			decodedMessage += ' ';
			i++; // Skip over one
		}
	}

	cout << endl << "Hill Case 1 Algorithm: " << decodedMessage << endl;
	Sleep(100);
	pthread_exit(0);
	return (NULL);
}

/*==========================================================================*/
void* hillCase2(void* args)
{
	string section_1, section_2;
	string decodedMessage = "";
	string origMessage = *reinterpret_cast<std::string*>(args);

	int indexToSplit = 0;
	for (unsigned int i = 1; i < origMessage.length(); i++) {
		if (isdigit(origMessage[i])) {
			indexToSplit = i - 1;
			break;
		}
	}

	// Create substrings
	section_1 = origMessage.substr(1, indexToSplit);
	section_2 = origMessage.substr(indexToSplit, origMessage.length() - 1);

	// Check if matrix has characters
	for (int i = 0; i < section_2.length(); i++)
	{
		if (isalpha(section_2.at(i))) {
			cout << endl << "Error: Characters found in Hill Case 1 Algorithm matrix." << endl;
			cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
			pthread_exit(0);
			return (NULL);
		}
	}

	// Trim white space
	stringstream sstream(section_2);
	int a, b, c, d, e, f, g, h, i;
	a = b = c = d = e = f = g = h = i = MAX;
	int error = MAX; /* Error Integer to check if there are too many numbers. */
	sstream >> a >> b >> c >> d >> e >> f >> g >> h >> i >> error;

	// Check if there are too many integers entered.
	if (error != MAX )
	{
		cout << endl << "Error: Too many integers found in Hill Case 2 Algorithm." << endl;
		cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
		pthread_exit(0);
		return (NULL);
	}
	// Check if all the entries in the 2x3 matrix are filled.
	if (a == MAX || b == MAX || c == MAX || d == MAX || e == MAX || f == MAX || g == MAX || h == MAX || i == MAX)
	{
		cout << endl << "Error: All entries of 2x3 matrix are not filled in Hill Case 2 Algorithm." << endl;
		cout << "Number of errors left:  " << 3 - (++errCounter) << endl;
		pthread_exit(0);
		return (NULL);
	}

	// Turn the sections into arrays
	char first, second, third;
	for (unsigned int i = 0; i < section_1.length();) {
		first = section_1[i];		// Get the first, second, and third characters
		second = section_1[i + 1];
		third = section_1[i + 2];

		if (isalpha(first) && isalpha(second) && isalpha(third)) {
			int x = alphaToNumMap[toupper(first)];	// Get the numeric value
			int y = alphaToNumMap[toupper(second)];
			int z = alphaToNumMap[toupper(third)];
			decodedMessage += numToAlphaMap[((a * x) + (b * y) + (c * z)) % 26]; 
			decodedMessage += numToAlphaMap[((d * x) + (e * y) + (f * z)) % 26];
			decodedMessage += numToAlphaMap[((g * x) + (h * y) + (i * z)) % 26];

			i += 3;	// Go to next triplet of characters
		}
		else {
			decodedMessage += ' ';
			i++; // Skip over one
		}
	}
	cout << endl << "Hill Case 2 Algorithm: " << decodedMessage << endl;
	Sleep(100);
	pthread_exit(0);
	return (NULL);
}