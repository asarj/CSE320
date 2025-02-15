#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h> // MADE CHANGE HERE - added missing import statement
#include <unistd.h> // MADE CHANGE HERE - added missing import statement
#include <string.h> // MADE CHANGE HERE - added missing import statement
#include <ctype.h>
#include <sgtty.h>
#include <signal.h>
#include <pwd.h>
#include <time.h> // MADE CHANGE HERE - added missing import statement

#include "toolsdir/sys5.h" // MADE CHANGE HERE - added missing import statement

#ifdef TMC
#include <toolsdir/ctools.h> // MADE CHANGE HERE - added missing import statement
#else
#include "toolsdir/ctools.h" // MADE CHANGE HERE - added missing import statement
#endif
#include "toolsdir/args.h" // MADE CHANGE HERE - added missing import statement
#include "toolsdir/menu.h" // MADE CHANGE HERE - added missing import statement
#include "toolsdir/mem.h" // MADE CHANGE HERE - added missing import statement

#include "rolofiles.h"
#include "rolodefs.h"
#include "datadef.h"
#include "helper.h" // MADE CHANGE HERE - added missing import statement
#include "debug.h" // MADE CHANGE HERE - added debugger header


static char rolodir[DIRPATHLEN];        /* directory where rolo data is */
static char filebuf[DIRPATHLEN];        /* stores result of homedir() */

int changed = 0;
int reorder_file = 0;
int rololocked = 0;
int in_search_mode = 0;


int rlist_free(Ptr_Rolo_List p){
  Ptr_Rolo_List temp;
  Ptr_Rolo_Entry temp2;
  char **ptrs;
  while(p != 0){
    temp = p;
    temp2 = p -> entry;
    ptrs = temp2 -> other_fields;
    p = p -> next;
    if(ptrs){
      free(ptrs);
    }
    free(temp2);
    free(temp);
  }
  return(0);
}

int roloexit (rval) int rval; // MADE CHANGE HERE - moved function up and added return identifier
{
if (rololocked) unlink(homedir(ROLOLOCK));
int i = 0;
while(cpystrlst[i] != 0){
  free(cpystrlst[i++]);
}
if(free_rolo_data){
  free(rolofiledata);
}
if(free_p_space){
  // debug("%s", "I am freeing pspace");
  free(p_space);
}
rlist_free(Begin_Rlist);
exit(rval);
}

void save_to_disk () // MADE CHANGE HERE - moved function up and added return identifier

/* move the old rolodex to a backup, and write out the new rolodex and */
/* a copy of the new rolodex (just for safety) */

{
    FILE *tempfp,*copyfp;
    char d1[DIRPATHLEN], d2[DIRPATHLEN];

    tempfp = fopen(homedir(ROLOTEMP),"w");
    copyfp = fopen(homedir(ROLOCOPY),"w");
    if (tempfp == NULL || copyfp == NULL) {
        fprintf(stderr,"Unable to write rolodex...\n");
        fprintf(stderr,"Any changes made have not been recorded\n");
        roloexit(-1);
    }
    if(write_rolo(tempfp,copyfp)){
      fprintf(stderr,"Unable to write rolodex...\n");
      fprintf(stderr,"Any changes made have not been recorded\n");
      roloexit(-1);
    }

    int x = fclose(tempfp);
    if(x != 0)
      roloexit(-1);
    int y = fclose(copyfp);
    if(y != 0)
      roloexit(-1);



    if (rename(strcpy(d1,homedir(ROLODATA)),strcpy(d2,homedir(ROLOBAK))) ||
        rename(strcpy(d1,homedir(ROLOTEMP)),strcpy(d2,homedir(ROLODATA)))) {
        fprintf(stderr,"Rename failed.  Revised rolodex is in %s\n",ROLOCOPY);
        roloexit(-1);
    }
    printf("Rolodex saved\n");
    sleep(1);
    changed = 0;
}

void save_and_exit (rval) int rval; // MADE CHANGE HERE - moved function up and added return identifier
{
if (changed) save_to_disk();
roloexit(rval);
}

char *rolo_emalloc (size) int size;

/* error handling memory allocator */

{
  char *rval;
  if (0 == (rval = malloc(size))) {
     fprintf(stderr,"Fatal error:  out of memory\n");
     save_and_exit(-1);
  }
  return(rval);
}

int cpyInd = 0;


char *copystr (s) char *s;

/* memory allocating string copy routine */


{
 char *copy;
 if(cpyInd == 0){
  for(int i = 0; i < 1000; i++){
    cpystrlst[i] = 0;
  }
 }
 if (s == 0) return(0);
 copy = rolo_emalloc(strlen(s) + 1);
 cpystrlst[cpyInd++] = copy;
 strcpy(copy,s);
 return(copy);
}


char *timestring ()

/* returns a string timestamp */

{
  char *s;
  long timeval;
  time(&timeval);
  s = ctime(&timeval);
  s[strlen(s) - 1] = '\0';
  return(copystr(s));
}


int user_interrupt () // MADE CHANGE HERE - added return identifier

/* if the user hits C-C (we assume he does it deliberately) */

{
  unlink(homedir(ROLOLOCK));
  fprintf(stderr,"\nAborting rolodex, no changes since last save recorded\n");
  return(-1);
}


void user_eof () // MADE CHANGE HERE - added return identifier

/* if the user hits C-D */

{
  unlink(homedir(ROLOLOCK));
  fprintf(stderr,"\nUnexpected EOF on terminal. Saving rolodex and exiting\n");
  save_and_exit(-1);
}

extern struct passwd *getpwnam();

char *home_directory (name) char *name;
{
  struct passwd *pwentry;
  if (0 == (pwentry = getpwnam(name))) return("");
  return(pwentry -> pw_dir);
}


char *homedir (filename) char *filename;

/* e.g., given "rolodex.dat", create "/u/massar/rolodex.dat" */
/* rolodir generally the user's home directory but could be someone else's */
/* home directory if the -u option is used. */

{
  nbuffconcat(filebuf,3,rolodir,"/",filename);
  return(filebuf);
}


char *libdir (filename) char *filename;

/* return a full pathname into the rolodex library directory */
/* the string must be copied if it is to be saved! */

{
  nbuffconcat(filebuf,3,ROLOLIB,"/",filename);
  return(filebuf);
}


int rolo_only_to_read () // MADE CHANGE HERE - added return identifier
{
  return(sFlag == 1 || n_option_counter > 0/* option_present(SUMMARYFLAG) || n_non_option_args() > 0 */);
}


void locked_action () // MADE CHANGE HERE - added return identifier
{
  if (uFlag == 1/*option_present(OTHERUSERFLAG)*/) {
     fprintf(stderr,"Someone else is modifying that rolodex, sorry\n");
     roloexit(-1); // MADE CHANGE HERE - valgrind fix
  }
  else {
     cathelpfile(libdir("lockinfo"),"locked rolodex",0);
     roloexit(-1); // MADE CHANGE HERE - valgrind fix
  }
}


int rolo_main (argc,argv) int argc; char *argv[];  // MADE CHANGE HERE - added return identifier

{
    int fd,in_use,read_only,rolofd;
    // Bool not_own_rolodex;
    char *user;
    FILE *tempfp;

    clearinit();
    clear_the_screen();

    /* parse the options and arguments, if any */

    // Modified argument parsing:
    char *options = ":uls";
    extern char *optarg;
    extern int optind, optopt, opterr;
    lFlag = -1;
    sFlag = -1;
    uFlag = -1;
    n_option_counter = 0;
    char c;
    // char *uName = NULL;
    // int global_options;
    /*
      * global_options & 0 = invalid
      * global_options & 1 = no args
      * global_options & 2 = l
      * global_options & 4 = s
      * global_options & 8 = u
      * global_options & 16 = search params
    */
    while((c = getopt(argc, argv, options)) != -1){
      if(argc < 2)
        break;
      switch(c){
        case 'l':
          if(lFlag == 1){
            fprintf(stderr,"duplicate option: %c\n", NOLOCKFLAG);
            roloexit(-1);
          }
          debug("%s", "L option is set");
          lFlag = 1;
          break;
        case 's':
          if(sFlag == 1){
            fprintf(stderr,"duplicate option: %c\n", SUMMARYFLAG);
            roloexit(-1);
          }
          debug("%s", "S option is set");
          sFlag = 1;
          break;
        case 'u':
          if(uFlag == 1){
            fprintf(stderr,"duplicate option: %c\n", OTHERUSERFLAG);
            roloexit(-1);
          }
          user = argv[optind];
          if(user == NULL || *user == '-' || user == 0){
            fprintf(stderr,"Illegal syntax using -u option\nusage: %s\n",USAGE);
            roloexit(-1);
          }
          uFlag = 1;
          debug("%s", "U option is set");
          debug("%s", user);
          break;
        case '?':
          fprintf(stderr,"illegal option\nusage: %s\n",USAGE);
          roloexit(-1);
          break;
        case 1:
          debug("%s\n", optarg);
          // debug("%s\n", optarg);
          break;
        default:
          debug("Default option reached: %d", c);
          break;
      }
    }
    int tempIndex = 0;
    for(; optind < argc; optind++){
        n_option_counter++;
        nonargs[tempIndex++] = argv[optind];
        // debug("extra arguments: %s\n", argv[optind]);
    }

    // switch (get_args(argc,argv,T,T)) {
    //     case ARG_ERROR :
    //       roloexit(-1);
    //     case NO_ARGS :
    //       break;
    //     case ARGS_PRESENT :
    //       if (ALL_LEGAL != legal_options(LEGAL_OPTIONS)) {
    //             fprintf(stderr,"illegal option\nusage: %s\n",USAGE);
    //             roloexit(-1);
    //       }
    // }

    /* find the directory in which the rolodex file we want to use is */


    // if (uFlag == 1) {
    //    if (NIL == (user = option_arg(OTHERUSERFLAG,1)) ||
    //        n_option_args(OTHERUSERFLAG) != 1) {
    //     // if(-1 == uFlag || ){}
    //       fprintf(stderr,"Illegal syntax using -u option\nusage: %s\n",USAGE);
    //       roloexit(-1); // MADE CHANGE HERE - valgrind fix
    //    }
    // }
    if (uFlag != 1){
       if (0 == (user = getenv("HOME"))) {
          fprintf(stderr,"Cant find your home directory, no HOME\n");
          roloexit(-1);
       }
    }

    if (uFlag == 1) {

       strcpy(rolodir,home_directory(user));
       if (*rolodir == '\0') {
          fprintf(stderr,"No user %s is known to the system\n",user);
          roloexit(-1);
       }
    }
    else strcpy(rolodir,user);

    /* is the rolodex readable? */

    if (0 != access(homedir(ROLODATA),R_OK)) {

       /* No.  if it exists and we cant read it, that's an error */

       if (0 == access(homedir(ROLODATA),F_OK)) {
          fprintf(stderr,"Cant access rolodex data file to read\n");
          roloexit(-1);
       }

       /* if it doesn't exist, should we create one? */

       if (uFlag == 1 /* option_present(OTHERUSERFLAG) */ ) {
          fprintf(stderr,"No rolodex file belonging to %s found\n",user);
          roloexit(-1);
       }

       /* try to create it */

       if (-1 == (fd = creat(homedir(ROLODATA),0644))) {
          fprintf(stderr,"couldnt create rolodex in your home directory\n");
          roloexit(-1);
       }

       else {
          close(fd);
          fprintf(stderr,"Creating empty rolodex...\n");
       }

    }

    /* see if someone else is using it */

    in_use = (0 == access(homedir(ROLOLOCK),F_OK));

    /* are we going to access the rolodex only for reading? */

    if (!(read_only = rolo_only_to_read())) {

       /* No.  Make sure no one else has it locked. */

       if (in_use) {
          locked_action();
       }

       /* create a lock file.  Catch interrupts so that we can remove */
       /* the lock file if the user decides to abort */

       if (!(lFlag == 1)/*!option_present(NOLOCKFLAG)*/) {
          if ((fd = open(homedir(ROLOLOCK),O_EXCL|O_CREAT,00200|00400)) < 0) {
             fprintf(stderr,"unable to create lock file...\n");
       roloexit(1); // MADE CHANGE HERE - valgrind fix
    }
          rololocked = 1;
          close(fd);
          signal(SIGINT, (void (*)(int)) user_interrupt); // MADE CHANGE HERE - added cast to second parameter
       }

       /* open a temporary file for writing changes to make sure we can */
       /* write into the directory */

       /* when the rolodex is saved, the old rolodex is moved to */
       /* a '~' file, the temporary is made to be the new rolodex, */
       /* and a copy of the new rolodex is made */

       if (NULL == (tempfp = fopen(homedir(ROLOTEMP),"w"))) {
           fprintf(stderr,"Can't open temporary file to write to\n");
           roloexit(-1);
       }
       fclose(tempfp);

    }

    allocate_memory_chunk(CHUNKSIZE);

    if (-1 == (rolofd = open(homedir(ROLODATA),O_RDONLY))) { // MADE CHANGE HERE - changed NULL to -1
        fprintf(stderr,"Can't open rolodex data file to read\n");
        roloexit(-1);
    }

    /* read in the rolodex from disk */
    /* It should never be out of order since it is written to disk ordered */
    /* but just in case... */

    if (!read_only) printf("Reading in rolodex from %s\n",homedir(ROLODATA));
    read_rolodex(rolofd);
    close(rolofd);
    if (!read_only) printf("%d entries listed\n",rlength(Begin_Rlist));
    if (reorder_file && !read_only) {
       fprintf(stderr,"Reordering rolodex...\n");
       rolo_reorder();
       fprintf(stderr,"Saving reordered rolodex to disk...\n");
       save_to_disk();
    }

    /* the following routines live in 'options.c' */

    /* -s option.  Prints a short listing of people and phone numbers to */
    /* standard output */

    if (sFlag == 1/*option_present(SUMMARYFLAG)*/) {
        print_short();
        roloexit(0);
    }

    /* rolo <name1> <name2> ... */
    /* print out info about people whose names contain any of the arguments */

    if (n_option_counter > 0/*n_non_option_args() > 0*/) {
       print_people();
       roloexit(0);
    }

    /* regular rolodex program */

    interactive_rolo();
    roloexit(0); // MADE CHANGE HERE - valgrind fix
    return 0;
}