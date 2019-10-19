#ifndef		_MISC_FATFS_H_
#define		_MISC_FATFS_H_

#include "fatfs.h"


FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
);

#endif	/* _MISC_FATFS_H_ */
		