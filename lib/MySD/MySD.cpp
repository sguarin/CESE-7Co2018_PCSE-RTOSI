/******************************************************************************

  Sebastián Guarino

  Distributed as-is; no warranty is given.
******************************************************************************/

#include <mySD.h>

/* Constructor */
void mySD::init() {
	/* initialize object members with defaults */
	/* Optionaly you can select SD.begin(CS_PIN) */
    if(!SD.begin()) {
    	status = C_STATUS_EMPTY;
        DEBUG_MYSD("Card Mount Failed");
        return;
    }
    status = C_STATUS_PRESENT;
}

bool mySD::appendLine(const char *data) {
	if (!outputFile) {
		if (!SD.exists(C_MYSD_FILENAME)) {
			outputFile = SD.open(C_MYSD_FILENAME, FILE_WRITE);
			DEBUG_MYSD("Open file %s in write mode\n", C_MYSD_FILENAME);
		} else {
			outputFile = SD.open(C_MYSD_FILENAME, FILE_APPEND);
			DEBUG_MYSD("Open file %s in append mode\n", C_MYSD_FILENAME);
		}
		if (!outputFile) {
			DEBUG_MYSD("Error opening Open file %s\n", C_MYSD_FILENAME);
			return false;
		}
	}

	if (outputFile.print(data) != strlen(data)) {
		DEBUG_MYSD("Error written less bytes than expected\n");
		return false;
	}
	// TODO investigate bug in SD library. Close shouldn't be necesary
	// or count N writes to close and reopen
	outputFile.close();
	DEBUG_MYSD("WRITE OK\n");
	return true;
}

String mySD::getDir(const String &dirname) {
	String listing;
	File root = SD.open(dirname);
	if (!root.isDirectory())
		return "";

	listing.concat("MEMORIA");
	File file = root.openNextFile();
	while (file) {
		if (!file.isDirectory()) {
			listing.concat("&");
			listing.concat(file.name());
			listing.concat(",");
			listing.concat(file.size());
			listing.concat(",");
			// TODO DATE/TIME here
			// must be implemented inside FS library
			listing.concat("");
		}
		file = root.openNextFile();
	}
	return listing;
}

String mySD::getUsedBytes() {
	uint64_t u = SD.usedBytes();
	return uint64ToString(u);
}

String mySD::getTotalBytes() {
	uint64_t u = SD.totalBytes();
	return uint64ToString(u);
}

String mySD::getFreeBytes() {
	uint64_t u = SD.totalBytes() - SD.usedBytes();
	return uint64ToString(u);
}

String mySD::uint64ToString(uint64_t input) {
	String result = "";
	uint8_t base = 10;

	do {
		char c = input % base;
		input /= base;

		if (c < 10)
			c +='0';
		else
			c += 'A' - 10;
		result = c + result;
	} while (input);
	return result;
}

mySD sd;

