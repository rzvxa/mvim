# MVIM
Multi VIM, light weight VIM configuration manager

## What is it?
It's a layer on top of vim for making managing and having multiple vimrc files easier.
You will create a new vim configuration using `mvim --new config_name` and it will create it's vimrc and vim directory inside of .mvim directory than you can launch vim with your config file using `mvim config_name` or running gvim using `mvim config_name -g`.

You can list all of your configs using `mvim ls`.
For removing a config use this `mvim --remove config_name`.

## Why?
I know almost anything you can achieve with mvim is possible to do with filetype plugins but i personally have a few problems with it.
I don't know about you guys but i use a handful of languages to work, I'm a Unity developer, i do ruby on rails as a freelancer, using python for quick scriptings, and C/C++ for performance dependent projects, I also have a passion for experiencing with rust.
It is hard to make one config file work with all of them out of the box, but worst thing happens when 2 plugins have a conflict with each other when you are writing say C and just want to copy a string from your ruby file. Now all the sudden there is 2 plugins for autocomplete in back ground and why should i care about loading a language server for my ruby? Now there is an eclimd running in background and it's a full blown ruby IDE inside my c IDE why?
With MVIM you can separate your config files and your vim instances are completely separate in both config file and vim directory that they are using and runtime directory.
Other thing is when i want to write a new project, in go for example. Now i have to config my vim to work with go too and now i have massive filetype plugins i have to get everywhere with me, but with MVIM you can have one simple config file. If you want you have use some `@path/to/include` to include other thing exactly where you want it, I don't like how source works in vim i want a c style including, just copy content of given file where i need it, actually do it twice if want to do it.

## What it can do?
With mvim you can have and manage more than a single vimrc / gvimrc / .vim directory.
MVIM config files are VimL, but it also has some added features witch work like a pre compiler, for now i only implemented @ for including other viml files, any vim file you put inside your ~/.mvim/share/ can get included in other viml files created by mvim, for example there is a file for your color scheme configs in ~/.mvim/share/common/dracula.vim
you can include it in your config like this:
@common/dracula

And it will write the contents of dracula.vim file instead if this line.

Also there is some constant variables like `__cdir__` for getting config directory vim is running on.
Try echom `__cdir__` in your mvim config file.

Honestly it's all for now, i have plans to add some special blocks to run pythonand maybe some other scripting languages like lua, and some kind of configuration file to add third party processors too, mvim will just pass the content to your program and the program can process it and return output and we are going to pass it into them like a chain and it's all will result in the config we will going to feed into vim like a modular transcompiler for vim language. It opens up doors for plugins completely based on these features.

The pre processor would not replace viml by any mean it "improve" it's syntax.
But there is long way to go, and i need feed backs from you guys so i can build a better vim for all of us.

## What i use it for?
Ok let me just explain what i do with it.
I have a few repos for vim configs i have, On some of my machines i want to have 2 languages on some i want only one, and on my laptop i want all of them.
With MVIM approach i can include each of them as a submodule inside a bigger module or just git clone each one of them in my `.mvim` directory.
I also have a share directory with some .vim files having general plugins like ctrl-p that i use in all my configs, some plugins for c-style languages. I also have some configuration for all of them like my color scheme and global shortcuts. I put all of them in separate files inside my share directory and i just include them where i want it to be.

And i have a config with nothing but goyo and things for focus mode, When i write short stories and editing my md files i use it and from start there is nothing but for example my note and i can even have configs that can open my notes at startup. so i just write `mvim notes` and here i'm looking at my notes how it should be.

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
Let's make a hello world config called hello_world.
Just call `mvim --new hello_world` to create a hello_world configuration in ~/.mvim directory.
Go to `~/.mvim/hello_worldvimrc` file and add `echom __cdir__` to it, now run vim with this configuration file using `mvim hello_world`.
Check the messages, it printed path to vim directory in use by vim instance.

## Plugin managers
All plugin managers should be compatible with MVIM but if you want to have different versions for plugins in diffrent configs i recommend you to use `vim-plug` i don't know about vundle or other plugin managers but you need to specify plugins directory for `vim-plug` i just use this:

	call plug#begin(__cdir__ . "/plugged")
	Plug 'prabirshrestha/vim-lsp'
	call plug#end()
and it will use mvim config directory for plugins.
If you know how to use other plugin managers like this just let me know!
