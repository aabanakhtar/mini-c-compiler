#ifndef ERROR_H
#define ERROR_H

#include <ostream> 

enum class ErrorMode 
{
    NO_ERR, ERR
};

void report_err(std::ostream& to, const std::string& what); 
extern ErrorMode get_err(); 

#endif