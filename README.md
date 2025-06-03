# LeafMeAlone - System Monitorowania i Nawadniania Roślin

LeafMeAlone to inteligentny system IoT oparty na ESP32, który umożliwia zdalne monitorowanie i kontrolę nawadniania roślin doniczkowych. System wykorzystuje komunikację MQTT do przesyłania danych telemetrycznych oraz odbierania poleceń sterujących.

## Funkcjonalność

- Monitorowanie wilgotności gleby
- Monitorowanie temperatury otoczenia
- Zdalne sterowanie nawadnianiem poprzez MQTT
- Wysyłanie danych telemetrycznych do serwera MQTT
- Automatyczne ponowne połączenie w przypadku utraty WiFi/MQTT
- Watchdog monitorujący połączenie

## Architektura systemu

System składa się z następujących komponentów:

1. **Controller** - zarządza diodą LED (symulującą urządzenie podlewające)
2. **MqttManager** - obsługuje komunikację WiFi i MQTT
3. **Sensors** - odczytuje dane z czujników (obecnie symulowane)
4. **Main Program** - integruje wszystkie komponenty

## Komunikacja MQTT

System wykorzystuje następujące tematy MQTT:

- `plant/{ID}/telemetry` - publikacja danych telemetrycznych
- `plant/{ID}/cmd/water` - subskrypcja komend podlewania
- `plant/{ID}/status` - publikacja informacji o statusie urządzenia

## Sposób działania

1. ESP32 łączy się z WiFi i brokerem MQTT.
2. Co 5 sekund odczytuje dane z czujników i wysyła je do brokera MQTT.
3. Nasłuchuje komend podlewania na odpowiednim temacie.
4. Po otrzymaniu komendy podlewania, włącza diodę LED (symulacja pompki) na określony czas.

## Lista zadań (TODO)

### Obsługa czujników
- [ ] Implementacja rzeczywistego czujnika wilgotności gleby (np. pojemnościowy)
- [ ] Implementacja czujnika temperatury (np. DS18B20)
- [ ] Implementacja czujnika nasłonecznienia (np. fotorezystor lub BH1750)
- [ ] Kalibracja czujników dla różnych typów roślin

### Obsługa pompki
- [ ] Dodanie sterownika pompki (np. tranzystor MOSFET lub przekaźnik)
- [ ] Implementacja zabezpieczeń przed zbyt długim czasem pompowania
- [ ] Dodanie pomiaru poziomu wody w zbiorniku
- [ ] Opcjonalne: implementacja elektrozaworu do precyzyjnego dozowania wody

### Ulepszenia oprogramowania
- [ ] Dodanie konfiguracji przez WiFi (captive portal)
- [ ] Implementacja zapisywania konfiguracji w pamięci EEPROM/SPIFFS
- [ ] Dodanie logiki automatycznego podlewania w oparciu o odczyty wilgotności
- [ ] Implementacja wykrywania różnych stanów awaryjnych (brak wody, awaria pompki)

### Zarządzanie energią
- [ ] Implementacja trybów oszczędzania energii (deep sleep)
- [ ] Dodanie obsługi zasilania bateryjnego
- [ ] Monitorowanie napięcia baterii

## Wymagania sprzętowe

- ESP32 DevKit
- Czujnik wilgotności gleby
- Czujnik temperatury
- Czujnik nasłonecznienia
- Pompka wody 5V lub przekaźnik do pompki 12V
- Zbiornik na wodę
- Kable połączeniowe

## Instalacja i konfiguracja

1. Sklonuj repozytorium
2. Zainstaluj platformę PlatformIO
3. Skonfiguruj dane WiFi i MQTT w pliku `main.cpp`
4. Zbuduj i wgraj program do ESP32
5. Podłącz czujniki i pompkę zgodnie ze schematem połączeń

## Schemat połączeń

TODO: Dodać schemat połączeń

## Licencja

MIT

## Autor

TODO: Dodać informację o autorze
