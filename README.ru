СБОРКА ПРОЕКТА
==============

* Сделать локальную копию git-репозитория и сменить текущую директорию

	[eland]/home/smironov/tmp% git clone ~smironov/proj/pure-usb.git/
	Cloning into pure-usb...
	done.
	[eland]/home/smironov/tmp% cd pure-usb/

* Установить переменную окружения CROSS\_COMPILE. Убедиться что путь к программе
  arm-none-eabi-gcc указан в переменной PATH.

	[eland]/home/smironov/tmp/pure-usb% setenv CROSS_COMPILE arm-none-eabi-

* Выполнить команду make. Создадутся файлы pure-usb-model.bin и
  pure-usb-board.bin.

	[eland]/home/smironov/tmp/pure-usb% make

ЗАПУСК НА МОДЕЛИ
================

* Убелиться что переменная TOP\_DESIGN установлена корректно. Запустить
  моделирование с помощью скрипта minibober (из текущей директории)

	[eland]/home/smironov/tmp/pure-usb% ./minibober pure-usb-model.bin



