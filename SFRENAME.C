#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
void do_rename(const char *oldname, const char *newname, const int attrib);
#define NORMAL	0
#define RHS		(FA_RDONLY | FA_HIDDEN | FA_SYSTEM)
#define HS		(FA_HIDDEN | FA_SYSTEM)

/* sfrename - toggle os/2 file names (on the current drive) between
	"ea data. sf" and "wp root. sf" and "ea_data._sf" and "wp_root._sf"
	so that DOS backup and disk defragmenting routines will handle them.
 */

int main(void)
	{
	int ea_space, ea_nospace, wp_space, wp_nospace;
	char *state[2] = {"doesn't exist", "exists"};

	printf("Sfrename version 1.0, Copyright Oct. 2, 1993 by\n"
			"Robert W. Babcock and WSS Division of DDC\n\n");

/* Check for existence of files (access returns 0 if file exists)
 */
	ea_space = !access("\\ea data. sf", 0);
	wp_space = !access("\\wp root. sf", 0);
	ea_nospace = !access("\\ea_data._sf", 0);
	wp_nospace = !access("\\wp_root._sf", 0);

	if(ea_space && wp_space && !ea_nospace && !wp_nospace)
		{
/* OS/2 names exist, insert underscores to make valid DOS names
 */
		do_rename("\\ea data. sf", "\\ea_data._sf", NORMAL);
		do_rename("\\wp root. sf", "\\wp_root._sf", NORMAL);
		}
	else if(!ea_space && !wp_space && ea_nospace && wp_nospace)
		{
/* DOS names exist, change to OS/2 form
 */
		do_rename("\\ea_data._sf", "\\ea data. sf", RHS);
		do_rename("\\wp_root._sf", "\\wp root. sf", HS);
		}
	else
		{
/* Unexpected combination of names found, print error message and exit
	without doing anything
 */
		printf("Expected file names not found:\n  EA DATA. SF %s\n"
				"  WP ROOT. SF %s\n  EA_DATA._SF %s\n  WP_ROOT._SF %s\n"
				"No file names changed\n", state[ea_space], state[wp_space],
				state[ea_nospace], state[wp_nospace]);
		return(1);
		}
	return(0);
	}

/* Rename files, change attributes, check for errors, print messages
 */
void do_rename(const char *oldname, const char *newname, const int attrib)
	{
	int old_archive;

	if(0 == rename(oldname, newname))
		printf("  %s renamed to %s\n", oldname, newname);
	else
		{
		printf("  Error, unable to rename %s to %s\n", oldname, newname);
		exit(1);
		}
	if(-1 == (old_archive = _chmod(newname, 0)))
		{
		printf("Unable to read attributes of %s\n", newname);
		exit(1);
		}
	old_archive &= FA_ARCH;
	if(-1 == _chmod(newname, 1, attrib | old_archive))
		{
		printf("Unable to change attributes of %s\n", newname);
		exit(1);
		}
	return;
	}
