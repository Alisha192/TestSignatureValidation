#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

// Генерация ключей
struct KeyPair {
	unsigned long long publicKey;
	unsigned long long privateKey;
};

KeyPair generateKeys() {
	KeyPair keys;
	srand(static_cast<unsigned int>(time(0)));
	keys.publicKey = rand() % 1000 + 1; // Генерация случайных значений для ключей
	keys.privateKey = rand() % 1000 + 1;
	return keys;
}

// Генерация хэша
unsigned long long hashData(const std::string& data) {
	unsigned long long hash = 0;
	for (char c : data) {
		hash += c; // Простая сумма кодов символов строки
	}
	return hash;
}

// Функция для формирования подписи
unsigned long long signData(const std::string& inputData, unsigned long long privateKey) {
	// Генерация хэша
	unsigned long long dataHash = hashData(inputData);

	// Подписание хэша с использованием закрытого ключа
	unsigned long long signature = dataHash * privateKey;

	return signature;
}

// Функция для проверки наличия подписи
bool hasSignature(const std::string& inputData) {
	if (inputData.find("Signature:") != std::string::npos) {
		return true;
	}
	return false;
}

// Функция для удаления цифровой подписи
std::string removeSignature(const std::string& inputData) {
	size_t pos = inputData.find_last_of('\n');
	return inputData.substr(0, pos);

}bool verifySignature(const std::string& inputData, unsigned long long signature, unsigned long long publicKey) {
	// Генерация хэша из входных данных
	unsigned long long dataHash = hashData(inputData);

	// Дешифрование подписи с использованием открытого ключа
	// Расшифрование подписи должно дать хэш исходных данных
	unsigned long long decryptedHash = (unsigned long long)(pow(signature, publicKey)) % (publicKey * publicKey);

	// Сравнение полученного дешифрованного хэша с фактическим хэшем
	return (decryptedHash == dataHash);
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cerr << "Использование: " << argv[0] << " <входной_файл> <выходной_файл>" << std::endl;
		return 1;
	}

	// Чтение данных из входного файла
	std::string inputFileName = argv[1];
	std::ifstream inputFile(inputFileName, std::ios::binary);
	if (!inputFile) {
		std::cerr << "Не удалось открыть файл для чтения: " << inputFileName << std::endl;
		return 1;
	}

	std::string inputData((std::istreambuf_iterator<char>(inputFile)), (std::istreambuf_iterator<char>()));
	inputFile.close();

	// Проверка наличия подписи
	bool signatureExists = hasSignature(inputData);
	if (signatureExists) {
		std::cout << "Цифровая подпись найдена\n";
		// Опционально: удаление цифровой подписи
		std::cout << "Удалить подпись?\n";
		std::cout << "Y(Да), N(нет)\n";
		char flag;
		std::cin >> flag;
		if (flag == 'Y') {
			std::string unsignedData = removeSignature(inputData);
			inputData = unsignedData;
		}
	}
	else {
		std::cout << "Цифровая подпись отсутствует\n";
	}

	// Генерация ключей
	KeyPair keys = generateKeys();

	// Формирование подписи
	unsigned long long signature = signData(inputData, keys.privateKey);

	// Вывод информации о подписи
	std::cout << "Сформирована цифровая подпись: " << signature << std::endl;

	// Проверка подписи
	bool isSignatureValid = signatureExists ? verifySignature(inputData, signature, keys.publicKey) : true;

	if (isSignatureValid) {
		std::cout << "Подпись верна\n";
	}
	else {
		std::cout << "Подпись недействительна\n";
	}

	// Запись результата в другой файл
	std::string outputFileName = argv[2];
	std::ofstream outputFile(outputFileName, std::ios::binary);
	outputFile << "Original Data: " << inputData << std::endl;
	outputFile << "Signature: " << signature << std::endl;
	outputFile << "Public Key: " << keys.publicKey << std::endl;
	outputFile.close();

	return 0;
}
