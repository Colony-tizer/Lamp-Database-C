#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MIN_LINE_LENGTH 9
#define PROPERTIES_COUNT 5
#define MAX_LENGTH_VALUE 64

int DB_SIZE = 0;

struct Lamp
{
    char name[MAX_LENGTH_VALUE];
    double voltage;
    double frequency;
    double capacity;
    char socleType[MAX_LENGTH_VALUE];
};

void printRecord(struct Lamp item)
{
	printf("Name: %s; Voltage: %f; Frequency: %f; Capacity: %f; Socle Type: %s\n", 
		item.name, item.voltage, item.frequency, item.capacity, item.socleType);
}

void printDb(struct Lamp* db)
{
	for (int i = 0; i < DB_SIZE; ++i)
		printRecord(db[i]);
}

struct Lamp parseLine(char* line)
{
	struct Lamp info;

	double numberVal;
	char* word;
	
	// splits the line by words separated by ";" and returns first one
	word = strtok(line, ";");
	
	for (int i = 0; i < PROPERTIES_COUNT && strcmp(word,"\0") != 0; ++i) {
		// there's strict order of data: (char[])NAME, (double)VOLTAGE, (double)FREQ, (double)CAP, (char[])TYPE
		if (i != 0 && i != PROPERTIES_COUNT - 1 && strcmp(word,"\0") != 0)
		{
			numberVal = abs(strtod(word, NULL));
		}
		else
		{
			numberVal = 0.0;
		}

		switch (i)
		{
			case 0:
				strcpy(info.name, word);
				break;
			case 1:
				info.voltage = numberVal;
				break;
			case 2:
				info.frequency = numberVal;
				break;
			case 3:
				info.capacity = numberVal;
				break;
			case 4:
				strcpy(info.socleType, word);
				
				// gets rid of the end of the line
				if (strlen(info.socleType) >= strlen(word))
					info.socleType[strlen(word)-1] = '\0';
				break;
		}
		// gets the next word split by ";"
		word = strtok(NULL, ";");
	}
	return info;
}

// opens input file named fileName
struct Lamp* parseFile(char* fileName)
{
	FILE *inputFile = fopen(fileName, "r");
	printf("Opening %s...\n", fileName);

	if (inputFile == NULL)
	{
		printf("Error while opening file.\n");
		perror("Error");
		return NULL;
	}

	// temp vars
	char* line = NULL;
	char ch;
	unsigned int lineLengthMax = 0, lineLength = 0, totalLines = 0, totalChars = 0; 

	printf("Reading file...\n");

	while ((ch = fgetc(inputFile)) != -1)
	{
		if (ch == '\n')
		{
			if (lineLength + 2 > lineLengthMax)
				lineLengthMax = lineLength;

			totalChars += lineLength;
			lineLength = 0;

			++DB_SIZE;
		}
		else 
		{
			++lineLength;
		}
	}
	if (totalChars > 0)
		++DB_SIZE;

	// the max line length could be lost if it is the last one 
	if (lineLength > lineLengthMax)
		lineLengthMax = lineLength;
	
	printf("Total lines: %d.\n", DB_SIZE);
	printf("Max line length: %d.\n", lineLengthMax);

	struct Lamp* db = (struct Lamp*)malloc(sizeof(struct Lamp) * DB_SIZE);

	printf("DB memory allocated.\n");

	char* lineBuf = (char*)malloc((size_t)lineLengthMax * sizeof(char));

	fclose(inputFile);
	// resetting the cursor
	inputFile = fopen(fileName, "r");
	int index = 0;

	printf("Opened a file...\n");

	while (fgets(lineBuf, lineLengthMax, inputFile))
	{
		if (strlen(lineBuf) == 0 || (strlen(lineBuf) < MIN_LINE_LENGTH))
			continue;
		db[index] = parseLine(lineBuf);
		++index;
	}

	printDb(db);
	return db;
}

void showHelp()
{
	printf("Program commands:\n"
		"\tSEARCH________________________________________\n"
		"\tSearch by Name: n <lamp_name>\n"
		"\tSearch by Voltage: v =/>/< <value>\n"
		"\tSearch by Frequency: f =/>/< <value>\n"
		"\tSearch by Capacity: c =/>/< <value>\n"
		"\tSearch by Socle Type: st <type_name>\n"
		"\tSORT__________________________________________\n"
		"\tSort all entries by Name: sn <asc|desc>\n"
		"\tSort all entries by Voltage: sv <asc|desc>\n"
		"\tSort all entries by Frequency: sf <asc|desc>\n"
		"\tSort all entries by Capacity: sc <asc|desc>\n"
		"\tSort all entries by SocleType: sst <asc|desc>\n"
		"\tShow all entries: s or all - show all entities\n"
		"\tExit: exit - Exit\n");
}

int isComparator(char* word)
{
	int res = -1;

	if (strcmp(word, "<") == 0 || strcmp(word, "<=") == 0|| strcmp(word, "=") == 0 || 
	   strcmp(word, "==") == 0 || strcmp(word, ">") == 0 || strcmp(word, ">-") == 0) 
		res = 0;

	return res;
}

char* getPropertyValue(struct Lamp record, char* property)
{
	char* value = malloc(sizeof(char) * MAX_LENGTH_VALUE);
		if (strcmp(property, "name") == 0)
			strcpy(value, record.name);
		if (strcmp(property, "voltage") == 0)
			snprintf(value, sizeof(value), "%g", record.voltage);
		if (strcmp(property, "frequency") == 0)
			snprintf(value, sizeof(value), "%g", record.frequency);
		if (strcmp(property, "capacity") == 0)
			snprintf(value, sizeof(value), "%g", record.capacity);
		if (strcmp(property, "socleType") == 0)
			strcpy(value, record.socleType);
	return value;
}

void searchItems(struct Lamp* db, char* searchValue, char* secondParameter, char* value)
{
	int foundEntries = 0;

	if (isComparator(secondParameter) == 0)
	{
		double threshold = strtod(value, NULL);
		int showFlag = 0;

		for (int i = 0; i < DB_SIZE; ++i)
		{
			showFlag = 0;
			double value = strtod(getPropertyValue(db[i], searchValue), NULL);

			if (secondParameter == "=" && value == threshold || 
				(strlen(secondParameter) > 1 && secondParameter[1] == '=' 
				 && value == threshold))
			{
				showFlag = 1;
			}
			if (secondParameter[0] == '>' && value > threshold) 
			{
				showFlag = 1;
			}
			else if (secondParameter[0] == '<' && value < threshold) 
			{
				showFlag = 1;
			}
			if (showFlag == 1) 
			{
				++foundEntries;
				printRecord(db[i]);
			}
		}
	}
	else
	{
		for (int i = 0; i < DB_SIZE; ++i)
		{
			if (strcmp(secondParameter, getPropertyValue(db[i], searchValue)) != 0)
				continue;
			++foundEntries;
			printRecord(db[i]);
		}
	}

	printf("Total found: %d\n", foundEntries);
}

void swap(struct Lamp** db, int leftIndex, int rightIndex)
{
	struct Lamp temp = (*db)[leftIndex];
	(*db)[leftIndex] = (*db)[rightIndex];
	(*db)[rightIndex] = temp;
}

// with bubble sort
void sortDb(struct Lamp** db, char* property, char* sortType) 
{	
	for (int i = 0; i < DB_SIZE - 1; ++i) {
		for (int j = 0; j < DB_SIZE - i - 1; ++j) 
		{
			int firstIndex = j;
			int secondIndex = j+1;
			
			if ((property == "name" || property == "socleType") && (strcmp(sortType,"asc") == 0) && strcmp(getPropertyValue((*db)[j],property), getPropertyValue((*db)[j+1],property)) > 0)
			{
				swap(db, firstIndex, secondIndex);
			}
			else if ((property == "name" || property == "socleType") && (strcmp(sortType,"desc") == 0) && strcmp(getPropertyValue((*db)[j],property), getPropertyValue((*db)[j+1],property)) < 0) 
			{
				swap(db, firstIndex, secondIndex);
			}
			else 
			{
				double leftVal = strtod(getPropertyValue((*db)[j],property), NULL);
				double rightVal = strtod(getPropertyValue((*db)[j+1],property), NULL);

				if ((strcmp(sortType,"asc") == 0) && leftVal > rightVal)
					swap(db, firstIndex, secondIndex);
				else if ((strcmp(sortType,"desc") == 0) && leftVal < rightVal)
					swap(db, firstIndex, secondIndex);
			}
		}
	}
}

int main(int argc, char **argv) {
	// the default filename
	char *fileName = "data.txt";

	if (argc > 1)
		fileName = argv[1];

	struct Lamp* db;
	db = parseFile(fileName);

	if (db == NULL)
	{
		printf("The %s file is empty. Please, check the source file and try again\n", fileName);
		return -1;
	}

	printf("Showing help...\n");
	showHelp();

	char command[512] = "";

	while (strcmp(command, "exit") != 0)
	{
		fgets(command, sizeof(command), stdin);

		if (strlen(command) > 1)
			command[strlen(command)-1] = '\0';

		// read three words of the query
		// first -- query to search, sort, exit or print
		// second -- string to search OR the compare sign OR the type of the sort
		// third -- number, in the case of the searching numeric values
		char* firstWord = strtok(command, " ");
		char* secondWord = strtok(NULL, " ");
		char* thirdWord = strtok(NULL, " ");
		
		int showFlag = 0;

		if (strcmp(firstWord, "n") == 0)
		{
			firstWord = "name";
			showFlag++;
		}
		else if (strcmp(firstWord, "v") == 0)
		{
			firstWord = "voltage";
			showFlag++;
		}
		else if (strcmp(firstWord, "f") == 0)
		{
			firstWord = "frequency";
			showFlag++;
		}
		else if (strcmp(firstWord, "c") == 0)
		{
			firstWord = "capacity";
			showFlag++;
		}
		else if (strcmp(firstWord, "st") == 0)
		{
			firstWord = "socleType";
			showFlag++;
		}

		if (showFlag == 1)
		{
			printf("%s %s %s\n", firstWord, secondWord, thirdWord);
			searchItems(db, firstWord, secondWord, thirdWord);
		}
		else 
		{
			printf("Sorting command");

			if (strcmp(firstWord, "sn") == 0 )
			{
				firstWord = "name";
				showFlag++;
			}
			else if (strcmp(firstWord, "sv") == 0)
			{
				firstWord = "voltage";
				showFlag++;
			}
			else if (strcmp(firstWord, "sf") == 0)
			{
				firstWord = "frequency";
				showFlag++;
			}
			else if (strcmp(firstWord, "sc") == 0)
			{
				firstWord = "capacity";
				showFlag++;
			} 
			else if (strcmp(firstWord, "sst") == 0)
			{
				firstWord = "socleType";
				showFlag++;
			}

			if (showFlag == 1)
			{
				printf("Sorting...\n");

				sortDb(&db, firstWord, secondWord);

				firstWord = "all";
			}
		}

		if (strcmp(firstWord, "s") == 0 || strcmp(firstWord, "all") == 0)
			printDb(db);

		else if (strcmp(firstWord, "exit") == 0) 
		{
				printf("Exiting...");
				strcpy(command, "exit");
		} 
		else if (showFlag == 0) {
			showHelp();
			printf("Probably there is a mistake in the command. Check the command and try again\n");
		}
		
	}
	return 0;
}
