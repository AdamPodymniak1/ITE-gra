# Wolfenstein-like FPS (C++ / Raylib)

Projekt gry typu **Wolfenstein 3D** napisanej w **C++** z użyciem biblioteki **Raylib** (renderowanie, input) oraz **nlohmann/json** (konfiguracja, mapy, menu i statystyki).

Gra wykorzystuje technikę **raycastingu**, aby symulować trójwymiarowe środowisko w stylu klasycznych FPS-ów z lat 90. **Raycasting** polega na "wystrzeliwywaniu" linii z pozycji gracza, by sprawdzać dystans między jego pozycją, a najbliższymi obiektami, rysując je w odpowiedniej skali.

---

## Funkcjonalności

* Silnik raycastingowy (pseudo-3D)
* Poruszanie się gracza (Strzałki i strayfing przez AD)
* Mapy tworzone i ładowane z plików JSON
* System fal
* Różnorodni przeciwnicy
* System upgradeów

---

## Wymagania

* Kompilator C++ obsługujący **C++17** (w naszym przypadku toolsy v145 z Visual Studio 2026)
* **Git**
* **vcpkg**

---

## Instalacja vcpkg

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh   # Linux / macOS
bootstrap-vcpkg.bat    # Windows
```

Można też dodać vcpkg do systemu (opcjonalnie, ale zalecane dla wygody):

```bash
./vcpkg integrate install
```

---

## Instalacja zależności (vcpkg)

```bash
./vcpkg install raylib nlohmann-json
```

---

## Autorstwo

Adam Podymniak, Dominik Marek, Adrian Malawski

---
