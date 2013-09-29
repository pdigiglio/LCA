#!/bin/bash - 
#===============================================================================
#
#          FILE: analisi.sh
# 
#         USAGE: ./analisi.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Paolo Di Giglio (p.digiglio@gmail.com), 
#  ORGANIZATION: 
#       CREATED: 29/09/2013 08:58
#      REVISION:  ---
#===============================================================================

# Treat unset variables as an error
set -o nounset

# Common functions which work either with 1-d and 2-d lattice
source ../analisi_common.sh

# Create "./global.h" file containing the user-chosen parameters in
# the section below
function create_global {
	clear
	echo "# Creo nuovo header!"
	echo "# PARAMETRI: B ${B} J ${J} N ${N} M ${M}"

	# creo l'header
	echo "/* File header contenente i parametri del sistema */

#ifndef  global_H
#define  global_H

#define N ${N}	/* numero atomi */
#define M ${M}	/* 2 * numero di Trotter */

#define J ${J}
#define B ${B}

#define MSR ${SWEEP}

#endif   /* ----- #ifndef global_H  ----- */" > ./global.h
}

#####################################################################
#
# SYSTEM PARAMETERS
#
#####################################################################
B=1
J=1
N=32
M=256
SWEEP=2000

# "root" of the path to move file in
root="/home/paolo/"
# data file
MAIN="data.dat"

# output file for variances and SDMs
OUT="var_sdom.dat"
#####################################################################

prova
