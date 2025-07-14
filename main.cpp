#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include <cstdlib>

using namespace std;

struct WeatherData {
    string city;
    string temperature;
};

bool downloadPage(const string& url, const string& destFile) {
#ifdef _WIN32
    string command = "curl -s \"" + url + "\" -o \"" + destFile + "\"";
#else
    string command = "curl -s '" + url + "' -o '" + destFile + "'";
#endif
    return system(command.c_str()) == 0;
}

WeatherData getWeatherData(const string& city) {
    string url = "https://sinoptik.ua/pogoda/" + city;
    string destFile = "weather.html";
    WeatherData data;
    data.city = city;

    if (!downloadPage(url, destFile)) {
        cerr << "\n[!] Не вдалося завантажити сторінку для міста: " << city << endl;
        return data;
    }

    ifstream f(destFile);
    if (!f.is_open()) {
        cerr << "\n[!] Не вдалося відкрити файл: " << destFile << endl;
        return data;
    }

    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();
    remove(destFile.c_str());

    smatch match;
    regex temperatureRegex(R"(<p class=\"today-temp\">(.*?)<)");
    if (regex_search(content, match, temperatureRegex)) {
        data.temperature = match[1];
    } else {
        data.temperature = "н/д";
    }

    return data;
}

void displayWeather(const WeatherData& data) {
    cout << "\nМісто: " << data.city << endl;
    cout << "Температура повітря: " << data.temperature << endl;
    cout << "-----------------------------" << endl;
}

int main() {
    vector<string> favoriteCities = {
        "kremenchuk", "odesa", "kyiv", "lviv", "berlin",
        "praha", "london", "warsaw", "chisinau", "paris"
    };

    cout << "Погода у улюблених містах:\n";
    for (const auto& city : favoriteCities) {
        displayWeather(getWeatherData(city));
    }

    string userCity;
    cout << "\nВведіть назву міста англійською: ";
    cin >> userCity;
    WeatherData userData = getWeatherData(userCity);
    cout << "\nПогода у вибраному місті:";
    displayWeather(userData);

    return 0;
}
