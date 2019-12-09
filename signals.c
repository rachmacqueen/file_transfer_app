#define _GNU_SOURCE
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "networking.h"
#include "logger.h"
#include "config.h"


// function for handling SIGINT, SIGTEM, and SIGQUIT signals
void sig_handler(int sig)
{
	int closed;
	printf("\nProcess terminated by user.. \n");
	kill(SIGCLD,getpid());		// send kill signal to any active child processes
	fflush(stdout);		// make sure everything makes it to the output
	closed = fcloseall();		// close all open files
	if(closed == EOF)
	{
		perror("An error occurred whilst closing files. \n");
	}
	printf("%d open files were detected and are now closed. The program will now exit..\n", closed);
	end_log();		// function for program termination logfile entry  --> found in logger.c
	exit(1);
}

// function for handling SIGHUP signal
struct server_config sig_hup(int sig)
{
	signal_log();		// function for signal handling logfile entry --> found in logger.c		
	check_server_config();		//check if config file exists/create file and default variables for setting.conf --> found in config.c file
	struct server_config sc;
	sc = load_server_config();		//load config variables in from settings.conf file --> found in config.c file	
	return sc;	
}
