/** @file eHutils.h
 *  @brief Some useful stuff (string parsing etc. )
 *
 *  @author Hang Su
 *  @date 2012-08
 */
#ifndef EHUTILS_H
#define EHUTILS_H

/** @brief Parse given string(e.g. "10, 5\0") to integer array
 *  @param csvstr null-terminated c string
 *  @param siz number of integers inside the string; if -1 is passed, actural 
 *  size will be calculated and stored in siz
 *  @return array of integers, memory is allocated for it
 */
int* parseCSVstr2int(const char* csvstr, int* siz, int offset = 0);

/** @brief Parse given string(e.g. "1.2, 3.4\0") to double precision array
 *  @param csvstr null-terminated c string
 *  @param siz amount of numbers inside the string; if -1 is passed, actural 
 *  size will be calculated and stored in siz
 *  @return array of numbers, memory is allocated for it
 */
double* parseCSVstr2double(const char* csvstr, int* siz);

#endif
