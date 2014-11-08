# author: Ivan Starodubtsev boors2@gmail.com
# makefile - build config of the ball_path test task
# ball_path - test task from the samsung ukraine r&d center

BANNER="=============================================="

QUIET?=@

VPATH=src

INCLUDE=-Iinc

SRCS=$(foreach dirname, $(VPATH), $(wildcard $(dirname)/*.c))
OBJS=$(foreach filename, $(notdir $(SRCS)), $(filename:.c=.o))

APP_NAME=ball_path
MAIN=main

CC=gcc
OPT=-std=c99 -ggdb3 $(INCLUDE)

# help
help:
	$(QUIET)echo $(BANNER)
	$(QUIET)echo "To build an $(APP_NAME) run:\
		make app"
	$(QUIET)echo "To clean an $(APP_NAME) run:\
		make clean"
	$(QUIET)echo $(BANNER)


# main build target
app: main
	$(QUIET)echo "Building $(APP_NAME) application"
	$(QUIET)$(CC) $(OPT) -o $(APP_NAME) $(MAIN).o $(OBJS)
	$(QUIET)rm -rf $(MAIN).o $(OBJS)
	$(QUIET)echo "Check the \"$(APP_NAME)\" application in $(shell pwd)"

main: $(OBJS)
	$(QUIET)$(CC) -c $(OPT) -o $(MAIN).o $(MAIN).c

clean:
	$(QUIET)echo "Cleaning $(APP_NAME) application"
	$(QUIET)rm -rf $(APP_NAME) $(MAIN).o $(OBJS)

%.o: %.c
	$(QUIET)$(CC) -c $(OPT) $< -o $@

