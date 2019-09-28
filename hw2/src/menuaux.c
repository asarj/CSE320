#include <stdio.h>
#include <ctype.h>

#include "sys5.h"

#ifdef TMC
#include <toolsdir/ctools.h>
#else
#include "toolsdir/ctools.h"
#endif
#include "toolsdir/args.h"
#include "toolsdir/menu.h"

#include "rolofiles.h"
#include "rolodefs.h"
#include "datadef.h"
#include "helper.h" // MADE CHANGE HERE - added missing import statement


int rolo_menu_yes_no (prompt,rtn_default,help_allowed,helpfile,subject)

  char *prompt;
  int rtn_default;
  int help_allowed;
  char *helpfile, *subject;

{
  int rval;
  reask :
  rval = menu_yes_no_abort_or_help (
              prompt,ABORTSTRING,help_allowed,rtn_default
           );
  switch (rval) {
    case MENU_EOF :
      user_eof();
      break;
    case MENU_HELP :
      cathelpfile(libdir(helpfile),subject,1);
      goto reask;
      break;
    default :
      return(rval);
      break;
  }
  return(rval); // MADE CHANGE HERE - added return statement
}


int rolo_menu_data_help_or_abort (prompt,helpfile,subject,ptr_response) // MADE CHANGE HERE - added return identifier

  char *prompt, *helpfile, *subject;
  char **ptr_response;

{
  int rval;
  reask :
  rval = menu_data_help_or_abort(prompt,ABORTSTRING,ptr_response);
  if (rval == MENU_HELP) {
     cathelpfile(libdir(helpfile),subject,1);
     goto reask;
  }
  return(rval);
}


int rolo_menu_number_help_or_abort (prompt,low,high,ptr_ival) // MADE CHANGE HERE - added return identifier

  char *prompt;
  int low,high,*ptr_ival;

{
  int rval;
  if (MENU_EOF == (rval = menu_number_help_or_abort (
                               prompt,ABORTSTRING,low,high,ptr_ival
                           )))
     user_eof();
  return(rval);
}
