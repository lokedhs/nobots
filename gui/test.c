#include <X11/Intrinsic.h>

void init_mainwin( int *, char ** );

extern XtAppContext app_context;

int main( int argc, char **argv )
{
  init_mainwin( &argc, argv );

  XtAppMainLoop( app_context );

  return 0;
}
