#include "error.h"

static ErrorMode state = ErrorMode::NO_ERR; 

void report_err(std::ostream& to, const std::string& what)
{
    to << "ERROR! " << what << "\n";
    state = ErrorMode::ERR;
}

ErrorMode get_err() 
{
    return state;
}