#include <stdio.h>
#include <stdlib.h>
#ifndef DATADEF_H
#define DATADEF_H
#endif

extern void save_and_exit();
extern void write_rolo (FILE *fp1, FILE *fp2);
extern void cathelpfile (char *filepath, char *helptopic, int clear);
extern void clearinit();
extern void clear_the_screen();
extern int read_rolodex(int fd);
extern int roloexit (int rval);
extern void rolo_search_mode (int field_index, char *field_name, char *search_string);
extern void rolo_add();
extern int select_field_to_search_by (int *ptr_index, char **ptr_name);
extern void save_to_disk ();
extern void user_eof();
extern int rlength(Ptr_Rolo_List rlist);
extern void rolo_reorder();
extern void print_short();
extern void print_people();
extern void interactive_rolo();
extern int rolo_menu_yes_no (char *prompt, int rtn_default, int help_allowed, char *helpfile, char *subject);
extern void any_char_to_continue ();
extern void display_entry (Ptr_Rolo_Entry entry);
extern void rolo_insert (Ptr_Rolo_List link, int (*compare)());
extern void rolo_peruse_mode (Ptr_Rolo_List first_rlink);
extern void rolo_update_mode (Ptr_Rolo_List rlink);
extern void rolo_delete (Ptr_Rolo_List link);
extern void display_entry_for_update (Ptr_Rolo_Entry entry);
extern int rolo_menu_data_help_or_abort (char *prompt, char *helpfile, char *subject, char **ptr_response);
extern void display_field_names ();
extern int rolo_menu_number_help_or_abort (char *prompt, int low, int high, int *ptr_ival);
extern char *string_upcase (char *astring);
extern char *string_downcase (char *astring);
extern int entry_action (Ptr_Rolo_List rlink);
extern void display_list_of_entries (Ptr_Rolo_List rlist);
extern void summarize_entry_list (Ptr_Rolo_List rlist, char *ss);
