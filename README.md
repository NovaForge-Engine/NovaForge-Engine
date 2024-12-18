# NovaForge-Engine
Руководство по установке:

Склонировать репозиторий git clone
Перейти на нужную ветку git checkout <имя ветки> или воспользоваться клиентом git например github desktop\
Установить CMake
После клонирования и установки на нужную вветку пишем: git submodule update --init --recursive --depth 1
Порядок работы с моно:

Заходим в директорию msvc внутри Mono
Находим mono.sln
Билдим проект для Debug и Release версии(опционально)
Создаем в корневой директории папку build/bin
Из директории msvc копируем include в build
Из директории msvc\build\sgen\x64\lib\Debug все содержимое в директорию build/bin
