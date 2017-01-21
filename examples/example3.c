/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io.h>
#include <ctr9/io/ctr_drives.h>

#include <stdio.h>

/*	This example goes over how to use file and console IO.
 */

int main()
{
	//using ctr_libctr9_init(), which a developer can override since it is a
	//weakly linked symbol, by default libctr9 initializes the IO and console
	//subsystems. FILE operations and printf should just work unless libctr9
	//encountered an error.

	//ctr_drives_check_ready checks to see that the given drive is working
	//properly. Returns true if the drive is ready and can be used.
	bool sd = ctr_drives_check_ready("SD:");
	bool ctrnand = ctr_drives_check_ready("CTRNAND:");
	bool twln = ctr_drives_check_ready("TWLN:");
	bool twlp = ctr_drives_check_ready("TWLP:");

	printf("SD: %d, CTRNAND: %d, TWLN: %d, TWLP: %d\n", sd, ctrnand, twln, twlp);

	//To open a file, use fopen, using the drive as the prefix for the path.
	FILE *file = fopen("SD:/tmp.txt", "r+b");
	if (file)
	{
		printf("File did open, now closing...\n");
		fclose(file);
	}

	//ctr_drives_chdrive can be used to change the default drive used
	ctr_drives_chdrive("CTRNAND:");

	file = fopen("/rw/sys/SecureInfo_A", "rb");
	if (file)
	{
		printf("SecureInfo_A was found, now closing...\n");
		fclose(file);
	}

	return 0;
}

