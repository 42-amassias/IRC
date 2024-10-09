#!/bin/sh

RESTORE='\033[0m'

RED='\033[00;31m'
GREEN='\033[00;32m'
YELLOW='\033[00;33m'
BLUE='\033[00;34m'
PURPLE='\033[00;35m'
CYAN='\033[00;36m'
LIGHTGRAY='\033[00;37m'

LRED='\033[01;31m'
LGREEN='\033[01;32m'
LYELLOW='\033[01;33m'
LBLUE='\033[01;34m'
LPURPLE='\033[01;35m'
LCYAN='\033[01;36m'
WHITE='\033[01;37m'

make_project()
{
	echo "${BLUE}MAKING PROJECT...${RESTORE}"
	make > /dev/null && echo "${GREEN}Project build success${RESTORE}" || echo "${RED}Project build error${RESTORE}"
}

make_project
ps aux | grep ircserv | grep valgrind | awk '{print$2}' | xargs kill
valgrind ./ircserv 6667 caca &
while inotifywait -e modify,close_write,move,move_self,create,delete,delete_self . $(find src include -type d)
do
	echo "${BLUE}Change detected reloading...${RESTORE}"
	ps aux | grep ircserv | grep valgrind | awk '{print$2}' | xargs kill
	sleep 1
	rm ircserv
	make_project
	valgrind ./ircserv 6667 caca &
done
