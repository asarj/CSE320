#include<stdio.h>
#include<stdlib.h>


void save_and_exit();
void write_rolo (FILE *fp1, FILE *fp2);
void cathelpfile (char *filepath, char *helptopic, int clear);
void clearinit();
void clear_the_screen();
int read_rolodex(int fd);
void user_eof();
int rlength(Ptr_Rolo_List rlist);
void rolo_reorder();
void print_short();
void print_people();
void interactive_rolo();
int rolo_menu_yes_no (char *prompt, int rtn_default, int help_allowed, char *helpfile, char *subject);
void any_char_to_continue ();
void display_entry (Ptr_Rolo_Entry entry);
void rolo_insert (Ptr_Rolo_List link, int (*compare)());
void rolo_peruse_mode (Ptr_Rolo_List first_rlink);
void rolo_update_mode (Ptr_Rolo_List rlink);
void rolo_delete (Ptr_Rolo_List link);
void display_entry_for_update (Ptr_Rolo_Entry entry);