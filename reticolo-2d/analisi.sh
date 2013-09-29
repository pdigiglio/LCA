#!/bin/bash - 
#===============================================================================
#
#          FILE: analisi.sh
# 
#         USAGE: './analisi.sh' or 'bash analisi.sh'
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
	echo "# Creo nuovo header!"
	echo "# PARAMETRI: B ${B} J ${J} N ${N} M ${M} T ${T} SWEEP ${SWEEP}"

	# creo l'header
	echo "/* File header contenente i parametri del sistema */

#ifndef  global_INC
#define  global_INC

#define N ${N}	/* numero atomi asse x */
#define M ${M}	/* numero atomi asse y */
#define T ${T}	/* 4 * numero di Trotter */

#define B ${B}
#define J ${J}

#define MSR ${SWEEP}

#endif   /* ----- #ifndef global_INC  ----- */" > ./global.h
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
T=256
SWEEP=2000

# "root" of the path to move file in
root="/home/paolo/Pubblici/Dropbox/tesi/data/3d/"
# data file
MAIN="data.dat"

# output file for variances and SDMs
OUT="var_sdom.dat"
#####################################################################

# path to take data file from
DIR=${root}"B${B}.N${N}.M${M}.T${T}/"

main
