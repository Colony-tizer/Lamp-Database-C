#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PROPERTIES_COUNT 5
#define MAX_LENGTH_VALUE 64
// глобальная переменная для описания размера базы данных
int DB_SIZE = 0;

struct Lamp {
    char name[MAX_LENGTH_VALUE];
    double voltage;
    double frequency;
    double capacity;
    char socleType[MAX_LENGTH_VALUE];
};

// выводим информацию о строке
void printRecord(struct Lamp item) {
	printf("Name: %s; Voltage: %f; Frequency: %f; Capacity: %f; Socle Type: %s\n", 
		item.name, item.voltage, item.frequency, item.capacity, item.socleType);
}
// выводим всю информацию о БД построчно
void printDb(struct Lamp* db) {
	for (int i = 0; i < DB_SIZE; ++i)
		printRecord(db[i]);
}
// парсим входную строку
struct Lamp parseLine(char* line) {
	// объяевляем неинициилизированный объект Lamp
	struct Lamp info;
	// временные переменные для хранения данных
	double numberVal;
	char* word;
	// разделяем всю строку по словам, разделённым символом ";" 
	// и берём первое слово
	word = strtok(line, ";");
	
	for (int i = 0; i < PROPERTIES_COUNT && strcmp(word,"\0") != 0; ++i) {
		// если порядковый номер слова не первый и не последний
		// и если слово не пустое
		if (i != 0 && i != PROPERTIES_COUNT - 1 && strcmp(word,"\0") != 0) {
			// переводим слово в положительное числовое значение с плавающей запятой
			numberVal = abs(strtod(word, NULL));
		} else
			// в противном случае выставляем значение 0
			numberVal = 0.0;
		// устанавливаем значения для каждого параметра
		switch (i) {
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
				// отрезаем пустой символ, если он есть
				// иначе это затрудняет поиск по слову
				if (strlen(info.socleType) >= strlen(word))
					info.socleType[strlen(word)-1] = '\0';
				break;
		}
		// берём следующее слово, разделённое ";"
		word = strtok(NULL, ";");
	}
	return info;
}

// открывает входной файл с именем fileName
// вовзращает массив из объектов Lamp
struct Lamp* parseFile(char* fileName) {
	FILE *inputFile = fopen(fileName, "r");
	printf("Opening %s...\n", fileName);
	// Если входной файл не существует или не открывается
	// выводим ошибку
	if (inputFile == NULL) {
		printf("Error while opening file.\n");
		perror("Error");
		return NULL;
	}
	// временные переменные для хранения буферных значений
	// при потоковом чтении файла
	char* line = NULL;
	char ch;
	unsigned int lineLengthMax = 0, lineLength = 0, totalLines = 0, totalChars = 0; 
	printf("Reading file...\n");
	// читаем файл посимвольное
	// каждый символ записываем в переменную ch
	// читаем, пока не достигнем конца файла
	while ((ch = fgetc(inputFile)) != -1) {
		if (ch == '\n') {
			// если достигли конца строки 
			// проверяем и устанавливаем максимальную длину строки
			if (lineLength + 2 > lineLengthMax)
				lineLengthMax = lineLength;
			totalChars += lineLength;
			lineLength = 0;
			// увеличиваем размер БД
			++DB_SIZE;
		} else
			++lineLength;
	}
	// увеличиваем количество строк на одну
	// чтобы не потерять информацию
	// так как индексация от нуля
	if (totalChars > 0)
		++DB_SIZE;
	if (lineLength > lineLengthMax)
		lineLengthMax = lineLength;
	
	printf("Total lines: %d.\n", DB_SIZE);
	printf("Max line length: %d.\n", lineLengthMax);
	// выделяем память для массива объектов Lamp
	struct Lamp* db = (struct Lamp*)malloc(sizeof(struct Lamp) * DB_SIZE);
	printf("DB memory allocated.\n");
	// выделяем память для буферной строки
	// чтобы разобрать пословно каждую строки и записать информацию в массив
	char* lineBuf = (char*)malloc((size_t)lineLengthMax * sizeof(char));
	// закрываем чтение файла
	fclose(inputFile);
	// снова открываем
	// чтобы курсор чтения попал в начало файла
	inputFile = fopen(fileName, "r");
	int index = 0;
	printf("Opened a file...\n");
	
	// читаем построчно, пока не достигнем конца файла
	while (fgets(lineBuf, lineLengthMax, inputFile)) {
		// если строка не пустая и длиной более 8 символов 
		// (минимальное количество символов для описания одного объекта)
		if (strlen(lineBuf) != 0 && (strlen(lineBuf) >= 9)) {
			// парсим строку и записываем в массив по индексу
			db[index] = parseLine(lineBuf);
			++index;
		}
	}
	// выводим получившийся массив
	printDb(db);
	return db;
	// parse lines and add info into an array
}
// Выводит подсказку
void showHelp() {
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
// определяет, является ли входное слово word компаратором (знаком сравнения)
int isComparator(char* word) {
	int ans = -1;
	if (strcmp(word, "<") == 0 || strcmp(word, "<=") == 0|| strcmp(word, "=") == 0|| 
	   strcmp(word, "==") == 0 || strcmp(word, ">") == 0 || strcmp(word, ">-") == 0) 
		ans = 0;
	return ans;
}
// возвращает строковое значение свойства объекта Lamp
char* getPropertyValue(struct Lamp record, char* property) {
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
// Выводит записи, удовлетворяющим условиям
void searchItems(struct Lamp* db, char* searchValue, char* secondParameter, char* value) {
	int foundEntries = 0;
	// Если второй аргумент -- символ сравнения, то работаем с числовыми данными
	if (isComparator(secondParameter) == 0) {
		// устанавливаем значение, с которым сравниваются элементы
		double threshold = strtod(value, NULL);
		int showFlag = 0;
		// проходим по всему массиву
		for (int i = 0; i < DB_SIZE; ++i) {
			showFlag = 0;
			double value = strtod(getPropertyValue(db[i], searchValue), NULL);
			// если во втором аргументе присутсвует знак "=", то
			// сравниваем значения на их совпадение
			if (secondParameter == "=" && value == threshold || 
				(strlen(secondParameter) > 1 && secondParameter[1] == '=' 
				 && value == threshold))
				showFlag = 1;
			// если знак сравнения '>' и текущее значение больше сравниваемого
			// то выводим элемент
			if (secondParameter[0] == '>' && value > threshold)
				showFlag = 1;
			// если знак сравнения '<' и текущее значение больше сравниваемого
			// то выводим элемент
			else if (secondParameter[0] == '<' && value < threshold)
				showFlag = 1;
			if (showFlag == 1) {
				// показываем текущий элемент
				// увеличиваем количество найденных элементов
				++foundEntries;
				printRecord(db[i]);
			}
		}
	} else {
		// имеем дело со строковыми данными
		for (int i = 0; i < DB_SIZE; ++i) {
			// ищем посимвольное совпадние 
			if (strcmp(secondParameter, getPropertyValue(db[i], searchValue)) == 0) {
				++foundEntries;
				printRecord(db[i]);
			}
		}
	}
	// показываем количество найденных записей
	printf("Total found: %d\n", foundEntries);
}
// Меняет местами два элементы с индексами leftIndex и rightIndex
void swap(struct Lamp** db, int leftIndex, int rightIndex) {
	struct Lamp temp = (*db)[leftIndex];
	(*db)[leftIndex] = (*db)[rightIndex];
	(*db)[rightIndex] = temp;
}
// Сортировка пузырьком
void sortDb(struct Lamp** db, char* property, char* sortType) {	
	for (int i = 0; i < DB_SIZE - 1; ++i) {
		for (int j = 0; j < DB_SIZE - i - 1; ++j) {
			int firstIndex = j;
			int secondIndex = j+1;
			
			if ((property == "name" || property == "socleType") && (strcmp(sortType,"asc") == 0) && strcmp(getPropertyValue((*db)[j],property), getPropertyValue((*db)[j+1],property)) > 0)
				swap(db, firstIndex, secondIndex);
			else if ((property == "name" || property == "socleType") && (strcmp(sortType,"desc") == 0) && strcmp(getPropertyValue((*db)[j],property), getPropertyValue((*db)[j+1],property)) < 0) {
				swap(db, firstIndex, secondIndex);
			}
			else {
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
	// выставляем название входного файла по умолчанию
	char *fileName = "data.txt";
	// изменяем название файла, если оно указано в параметрах
	if (argc > 1) {
		fileName = argv[1];
	}
	
	// загружаем информацию из файла в базу данных
	struct Lamp* db;
	db = parseFile(fileName);
	// выходим из программы, если файл пустой или отсутсвует
	if (db == NULL) {
		printf("The %s file is empty. Please, check the source file and try again\n", fileName);
		return -1;
	}
	printf("Showing help...\n");
	showHelp();
	// переменная для запроса
	char command[512] = "";
	// цикл до тех пор, пока пользователь не введёт директиву выхода
	while (strcmp(command, "exit") != 0) {
		// читаем команду пользователя
		fgets(command, sizeof(command), stdin);
		// замыкаем строку команды пустым символом
		if (strlen(command) > 1)
			command[strlen(command)-1] = '\0';
		// читаем три слова команды
		// первое -- команда: поиск, сортировка, выход или показать все строки
		// второе -- тип команды: знак сравнения или указание сортироки по возрастанию/убыванию
		// третье -- значение для команды поиска
		char* firstWord = strtok(command, " ");
		char* secondWord = strtok(NULL, " ");
		char* thirdWord = strtok(NULL, " ");
		
		int showFlag = 0;
		// команды поиска по свойству объекта
		// если команда поиска по названию
		if (strcmp(firstWord, "n") == 0) {
			// то изменяем первое слово на название искомого свойства
			firstWord = "name";
			// указываем, что следует вывести базу данных в конце выполнения команды
			showFlag++;
		} else if (strcmp(firstWord, "v") == 0) {
			// принцип тот же
			firstWord = "voltage";
			showFlag++;
		} else if (strcmp(firstWord, "f") == 0) {
			firstWord = "frequency";
			showFlag++;
		} else if (strcmp(firstWord, "c") == 0) {
			firstWord = "capacity";
			showFlag++;
		} else if (strcmp(firstWord, "st") == 0) {
			firstWord = "socleType";
			showFlag++;
		}
		// если была указана команда поиска, то выполняем поиск
		if (showFlag == 1) {
			printf("%s %s %s\n", firstWord, secondWord, thirdWord);
			searchItems(db, firstWord, secondWord, thirdWord);
		}
		else {
			printf("Sorting command");
			// если указана команда сортировки по свойству объекта
			if (strcmp(firstWord, "sn") == 0 ) {
				// то изменяем первое слово на название искомого свойства
				firstWord = "name";
				showFlag++;
			} else if (strcmp(firstWord, "sv") == 0) {
				firstWord = "voltage";
				showFlag++;
			} else if (strcmp(firstWord, "sf") == 0) {
				firstWord = "frequency";
				showFlag++;
			} else if (strcmp(firstWord, "sc") == 0) {
				firstWord = "capacity";
				showFlag++;
			} else if (strcmp(firstWord, "sst") == 0) {
				firstWord = "socleType";
				showFlag++;
			}
			if (showFlag == 1) {
				printf("Sorting...\n");
				// выполненяем сортировку
				sortDb(&db, firstWord, secondWord);
				// указываем, что необходимо вывести БД на экран
				firstWord = "s";
			}
		}
		// команда вывода БД на экран
		if (strcmp(firstWord, "s") == 0 || strcmp(firstWord, "all") == 0)
			printDb(db);
		// команда выхода из программы
		else if (strcmp(firstWord, "exit") == 0) {
				printf("Exiting...");
				strcpy(command, "exit");
		// выполняется в том случае, если указана неверная команада
		} else if (showFlag == 0) {
			showHelp();
			printf("Probably there is a mistake in the command. Check the command and try again\n");
		}
		
	}
	return 0;
}