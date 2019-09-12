# MVIM
Multi VIM, light weight VIM configuration manager

## What is it?
It's a layer on top of vim for making managing and having multiple vimrc files easier.
You will create a new vim configuration using `mvim --new config_name` and it will create it's vimrc and vim directory inside of .mvim directory than you can launch vim with your config file using `mvim config_name` or running gvim using `mvim config_name -g`.

You can list all of your configs using `mvim ls`.
For removing a config use this `mvim --remove config_name`.

## What it can do?
With mvim you can have and manage more than a single vimrc / gvimrc / .vim directory.
MVIM config files are VimL, but it also has some added fetures witch work like a pre compiler, for now i only implemented @ for including other viml files, any vim file you put inside your ~/.mvim/share/ can get included in other viml filescreated by mvim, for example there is a file for your color scheme configs in ~/.mvim/share/common/dracula.vim
you can include it in your config like this:
@common/dracula

and it will write the contents of dracula.vim file instead if this line.

Also there is some constant variables like `__cdir__` for getting config directory vim is running on.
try echom `__cdir__` in your mvim config file.

Honestly it's all for now, i have plans to add some special blocks to run pythonand maybe some other scripting languages like lua, and some kind of configuration file to add third party processors too, mvim will just pass the content to your program and the program can process it and return output and we are going to pass it into them like a chain and it's all will result in the config we will going to feed into vim like a modular transcompiler for vim language. It opens up doors for plugins completly based on these features.

The pre processor would not replace viml by any mean it "improve" it's syntax.
But there is long way to go, and i need feed backs from you guys so i can build a better vim for all of us.

## How to install
For now you have to build it yourself!

### Dependencies
* clang / gcc
* pcre

First clone the repo anywhere you like using `git clone https://github.com/aliagamon/mvim.git`

Now cd into mvim `cd mvim`
Make it `make`
Install it `sudo make intall`

## Hello, world
Let's make a hellow world config called hello_world.
Just call `mvim --new hello_world` to create a hello_world configuration in ~/.mvim directory.
go to "~/.mvim/hello_worldvimrc" file and add `echom \__cdir\__` to it, now run vim with this configuration file using `mvim hello_world`.
Check the messages, it printed path to vim directory in use by vim instance.
