# NovaForge-Engine
Руководство по установке:
1) Склонировать репозиторий `git clone`
2) Перейти на нужную ветку `git checkout <имя ветки>` или воспользоваться клиентом git например github desktop\
3) Установить CMake
4) После клонирования и установки на нужную вветку пишем: `git submodule update --init --recursive --depth 1`

Порядок работы с моно:
1) Заходим в директорию msvc внутри Mono
2) Находим mono.sln
3) Билдим проект для Debug и Release версии(опционально)
4) Создаем в корневой директории папку build/bin
5) Из директории msvc копируем include в build
6) Из директории msvc\build\sgen\x64\lib\Debug все содержимое в директорию build/bin
