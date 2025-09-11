# About
Neocynamonka (ncm) is a simple tool for network monitoring it constantly pings
devices in specified in config and displays Round Trip Time or time since device
was Last Seen

# Building

## Arch
``` sh
sudo pacman -S git base-devel ncurses bison make
git clone https://github.com/2lotraugutt/neocynamonka
cd neocynamonka
make
```

## Debian
``` sh
sudo apt install gcc make bison git libncurses-dev
git clone https://github.com/2lotraugutt/neocynamonka
cd neocynamonka
make
```


