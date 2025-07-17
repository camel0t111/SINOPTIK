#include <iostream> // бібліотека для вводу-виводу типу cout, cin
#include <string>   // щоб працювати зі стрічками
#include <fstream>  // щоб зчитати HTML файл після завантаження
#include <regex>    // регулярні вирази, щоб витягнути температуру з html
#include <vector>   // щоб зробити список улюблених міст
#include <cstdlib>  // для system("curl ...")

using namespace std; // щоб не писати кожен раз std:: перед cout, string і т.д.

// структура для зберігання даних по погоді
struct WeatherData {
    string city;           // назва міста
    string temperature;    // температура (типу "+18")
};

// функція щоб завантажити сторінку з sinoptik.ua і зберегти її у файл
bool downloadPage(const string& url, const string& destFile) {
#ifdef _WIN32 // якщо компілюємо під вінду
    string command = "curl -s \"" + url + "\" -o \"" + destFile + "\""; // командка для cmd
#else // якщо не вінда (мака або лінух)
    string command = "curl -s '" + url + "' -o '" + destFile + "'"; // командка для bash
#endif
    return system(command.c_str()) == 0; // запускаємо curl, повертає true якщо все ок
}

// функція яка бере назву міста і повертає структуру з його температурою
WeatherData getWeatherData(const string& city) {
    string url = "https://sinoptik.ua/pogoda/" + city; // будуємо url на основі міста
    string destFile = "weather.html"; // тимчасовий файл для збереження HTML
    WeatherData data;
    data.city = city; // записали місто в структуру

    if (!downloadPage(url, destFile)) { // пробуємо скачати сторінку
        cerr << "\n[!] Не вдалося завантажити сторінку для міста: " << city << endl;
        return data; // якщо помилка — просто повертаємо пусті дані
    }

    ifstream f(destFile); // відкриваємо файл для читання
    if (!f.is_open()) { // перевірка, чи точно відкрився
        cerr << "\n[!] Не вдалося відкрити файл: " << destFile << endl;
        return data;
    }

    // читаємо весь HTML одразу в одну стрічку (бо по рядках може не знайти)
    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close(); // закриваєм файл бо вже не треба
    remove(destFile.c_str()); // видаляєм тимчасовий файл (шоб не залишався після запуску)

    smatch match; // обʼєкт де буде результат пошуку по regex
    regex temperatureRegex(R"(<p class=\"today-temp\">(.*?)<)"); // шаблон для витягування темпи (в sinoptik такий клас у них)
    if (regex_search(content, match, temperatureRegex)) {
        data.temperature = match[1]; // якщо знайдено — записуєм у структуру
    } else {
        data.temperature = "н/д"; // якщо не знайдено — ставимо "нема даних"
    }

    return data; // повертаєм структуру з містом і температурою
}

// функція яка просто красиво виводить погоду на екран
void displayWeather(const WeatherData& data) {
    cout << "\nМісто: " << data.city << endl;
    cout << "Температура повітря: " << data.temperature << endl;
    cout << "-----------------------------" << endl;
}

int main() {
    // список 10 улюблених міст (можеш змінити на свої, аби працювало)
    vector<string> favoriteCities = {
        "kremenchuk", "odesa", "kyiv", "lviv", "berlin",
        "praha", "london", "warsaw", "chisinau", "paris"
    };

    // показуємо температуру в кожному з цих міст
    cout << "Погода у улюблених містах:\n";
    for (const auto& city : favoriteCities) {
        displayWeather(getWeatherData(city)); // викликаємо функцію і одразу виводимо
    }

    string userCity; // сюди введе користувач своє місто
    cout << "\nВведіть назву міста англійською: ";
    cin >> userCity; // отримуємо від користувача
    WeatherData userData = getWeatherData(userCity); // шукаємо температуру
    cout << "\nПогода у вибраному місті:";
    displayWeather(userData); // виводимо результат

    return 0; // стандартний кінець програми
}
